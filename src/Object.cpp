/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Object.h"

#include <stack>
#include "Scene.h"
#include "Camera.h"

using namespace std;

namespace Magnum {

Object* Object::setParent(Object* parent) {
    /* Skip if nothing to do or this is scene */
    if(_parent == parent || _parent == this) return this;

    /* Add the object to children list of new parent */
    if(parent != nullptr) {

        /* Only Fry can be his own grandfather */
        Object* p = parent;
        while(p != nullptr && p->parent() != p) {
            if(p == this) return this;
            p = p->parent();
        }

        parent->_children.insert(this);
    }

    /* Remove the object from old parent children list */
    if(_parent != nullptr)
        _parent->_children.erase(this);

    /* Set new parent */
    _parent = parent;

    setDirty();
    return this;
}

Matrix4 Object::absoluteTransformation(Camera* camera) {
    /* Shortcut for absolute transformation of camera relative to itself */
    if(camera == this) return Matrix4();

    Matrix4 t = _transformation;

    Object* p = parent();
    while(p != nullptr) {
        t = p->transformation()*t;

        /* We got to the scene, multiply with camera matrix */
        if(p->parent() == p) {
            if(camera) {
                CORRADE_ASSERT(camera->scene() == scene(), "Object::absoluteTransformation(): the camera is not part of the same scene as object!", t);
                t = camera->cameraMatrix()*t;
            }

            break;
        }

        p = p->parent();
    }

    CORRADE_ASSERT(p != nullptr || camera == nullptr, "Object::absoluteTransformation(): the object is not part of camera scene!", t);

    return t;
}

Object::~Object() {
    /* Remove the object from parent's children */
    setParent(nullptr);

    /* Delete all children */
    while(!_children.empty())
        delete *_children.begin();
}

Scene* Object::scene() {
    /* Goes up the family tree until it finds object which is parent of itself
       (that's the scene) */
    Object* p = parent();
    while(p != nullptr) {
        if(p->parent() == p) return static_cast<Scene*>(p);
        p = p->parent();
    }

    return nullptr;
}

Object* Object::setTransformation(const Matrix4& transformation) {
    if(_parent == this) return this;

    _transformation = transformation;
    setDirty();
    return this;
}

void Object::setDirty() {
    /* The object (and all its children) are already dirty, nothing to do */
    if(dirty) return;

    dirty = true;

    /* Make all children dirty */
    for(set<Object*>::iterator it = _children.begin(); it != _children.end(); ++it)
        (*it)->setDirty();
}

void Object::setClean() {
    /* The object (and all its parents) are already clean, nothing to do */
    if(!dirty) return;

    /* Collect all parents */
    stack<Object*> objects;
    Object* p = this;
    for(;;) {
        objects.push(p);

        /* Stop on root object / scene / clean object */
        if(p->parent() == nullptr || p->parent() == p || !p->parent()->isDirty())
            break;

        p = p->parent();
    }

    /* Call setClean(const Matrix4&) for every parent and also this object */
    Object* o = objects.top();
    objects.pop();
    Matrix4 absoluteTransformation = o->absoluteTransformation();
    o->clean(absoluteTransformation);
    while(!objects.empty()) {
        o = objects.top();
        objects.pop();
        absoluteTransformation = absoluteTransformation*o->transformation();
        o->clean(absoluteTransformation);
    }
}

}
