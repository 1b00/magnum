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

#include "Camera.h"
#include "Framebuffer.h"
#include "Scene.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class MatrixType> MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Vector2& projectionScale, const Math::Vector2<GLsizei>& viewport) {
    /* Don't divide by zero / don't preserve anything */
    if(projectionScale.x() == 0 || projectionScale.y() == 0 || viewport.x() == 0 || viewport.y() == 0 || aspectRatioPolicy == AspectRatioPolicy::NotPreserved)
        return MatrixType();

    Vector2 relativeAspectRatio = Vector2::from(viewport)*projectionScale;

    /* Extend on larger side = scale larger side down
       Clip on smaller side = scale smaller side up */
    return Camera<MatrixType::Size-1>::aspectRatioScale(
        (relativeAspectRatio.x() > relativeAspectRatio.y()) == (aspectRatioPolicy == AspectRatioPolicy::Extend) ?
        Vector2(relativeAspectRatio.y()/relativeAspectRatio.x(), 1.0f) :
        Vector2(1.0f, relativeAspectRatio.x()/relativeAspectRatio.y()));
}

/* Explicitly instantiate the templates */
template Matrix3 aspectRatioFix<Matrix3>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);
template Matrix4 aspectRatioFix<Matrix4>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);

}
#endif

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::Camera(ObjectType* parent): ObjectType(parent), _aspectRatioPolicy(AspectRatioPolicy::NotPreserved) {}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setViewport(const Math::Vector2<GLsizei>& size) {
    Framebuffer::setViewport(Math::Vector2<GLint>{0, 0}, size);

    _viewport = size;
    fixAspectRatio();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::clean(const MatrixType& absoluteTransformation) {
    ObjectType::clean(absoluteTransformation);

    _cameraMatrix = absoluteTransformation.inverted();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::draw() {
    SceneType* s = this->scene();
    CORRADE_ASSERT(s, "Camera: cannot draw without camera attached to scene", );

    Framebuffer::clear();

    /* Recursively draw child objects */
    drawChildren(s, cameraMatrix());
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::drawChildren(ObjectType* object, const MatrixType& transformationMatrix) {
    for(ObjectType* i = object->firstChild(); i; i = i->nextSibling()) {
        /* Transformation matrix for the object */
        MatrixType matrix = transformationMatrix*i->transformation();

        /* Draw the object and its children */
        i->draw(matrix, static_cast<CameraType*>(this));
        drawChildren(i, matrix);
    }
}

void Camera2D::setProjection(const Vector2& size) {
    /* Scale the volume down so it fits in (-1, 1) in all directions */
    rawProjectionMatrix = Matrix3::scaling(2.0f/size);

    fixAspectRatio();
}

void Camera3D::setOrthographic(const Vector2& size, GLfloat near, GLfloat far) {
    _near = near;
    _far = far;

    Vector2 xyScale = 2.0f/size;
    GLfloat zScale = 2.0f/(near-far);

    rawProjectionMatrix = Matrix4(
        xyScale.x(),    0.0f,           0.0f,           0.0f,
        0.0f,           xyScale.y(),    0.0f,           0.0f,
        0.0f,           0.0f,           zScale,         0.0f,
        0.0f,           0.0f,           near*zScale-1,  1.0f
    );

    fixAspectRatio();
}

void Camera3D::setPerspective(GLfloat fov, GLfloat near, GLfloat far) {
    _near = near;
    _far = far;

    GLfloat xyScale = 1.0f/tan(fov/2); /* == near/size */
    GLfloat zScale = 1.0f/(near-far);

    rawProjectionMatrix = Matrix4(
        xyScale,    0.0f,       0.0f,                   0.0f,
        0.0f,       xyScale,    0.0f,                   0.0f,
        0.0f,       0.0f,       (far+near)*zScale,     -1.0f,
        0.0f,       0.0f,       (2*far*near)*zScale,    0.0f
    );

    fixAspectRatio();
}

/* Explicitly instantiate the templates */
template class Camera<Matrix3, Vector2, Object2D, Scene2D, Camera2D>;
template class Camera<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;

}}
