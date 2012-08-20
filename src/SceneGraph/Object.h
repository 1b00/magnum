#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
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

/** @file
 * @brief Class Magnum::SceneGraph::Object, Magnum::SceneGraph::Object2D, Magnum::SceneGraph::Object3D
 */

#include <Containers/LinkedList.h>

#include "Magnum.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@todo User-specified Object implementation:
- for front-to-back sorting, LoD changes etc.
- for different parent/children implementation (e.g. no std::set, direct
  access to scene etc.)
- for using doubles/halves instead of floats
- for using quat + position instead of matrices (where (asymmetric) scaling is
  not needed)
*/

/**
 * @brief Base for all positioned objects
 *
 * @todo Transform transformation when changing parent, so the object stays in
 * place.
 */
template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> class SCENEGRAPH_EXPORT Object: public Corrade::Containers::LinkedList<ObjectType>, public Corrade::Containers::LinkedListItem<ObjectType, ObjectType> {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Object(const Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>& other) = delete;
    Object(Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>&& other) = delete;
    Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>& operator=(const Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>& other) = delete;
    Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>& operator=(Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>&& other) = delete;
    #endif

    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets all transformations to their default values.
         */
        inline Object(ObjectType* parent = nullptr): dirty(true) {
            setParent(parent);
        }

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        virtual inline ~Object() {}

        /** @{ @name Scene hierarchy */

        /** @brief Whether this object is scene */
        virtual inline bool isScene() const { return false; }

        /**
         * @brief %Scene
         * @return If the object is not assigned to any scene, returns nullptr.
         */
        SceneType* scene();

        /** @brief Parent object or `nullptr`, if this is root object */
        inline ObjectType* parent() { return Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::list(); }

        /** @brief Previous sibling object or `nullptr`, if this is first object */
        inline ObjectType* previousSibling() { return Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::previous(); }

        /** @brief Next sibling object or `nullptr`, if this is last object */
        inline ObjectType* nextSibling() { return Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::next(); }

        /** @brief Whether this object has children */
        inline bool hasChildren() const { return !Corrade::Containers::LinkedList<ObjectType>::isEmpty(); }

        /** @brief First child object or `nullptr`, if this object has no children */
        inline ObjectType* firstChild() { return Corrade::Containers::LinkedList<ObjectType>::first(); }

        /** @brief Last child object or `nullptr`, if this object has no children */
        inline ObjectType* lastChild() { return Corrade::Containers::LinkedList<ObjectType>::last(); }

        /** @brief Set parent object */
        ObjectType* setParent(ObjectType* parent);

        /*@}*/

        /** @{ @name Object transformation
         *
         * All transformations (except absoluteTransformation()) are relative
         * to parent.
         */

        /** @brief Transformation type */
        enum class Transformation: char {
            /** Global transformation, applied after all other transformations. */
            Global = 0x00,

            /** Local transformation, applied before all other transformations. */
            Local = 0x01
        };

        /** @brief Transformation */
        inline MatrixType transformation() const {
            return _transformation;
        }

        /**
         * @brief Absolute transformation
         *
         * Returns absolute transformation matrix relative to the camera or
         * root object, if no camera is specified. If the camera is specified,
         * it should be part of the same scene as object.
         *
         * Note that the absolute transformation is computed from all parent
         * objects every time it is asked, unless this function is
         * reimplemented in a different way.
         */
        virtual MatrixType absoluteTransformation(CameraType* camera = nullptr);

        /** @brief Set transformation */
        ObjectType* setTransformation(const MatrixType& transformation);

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         */
        inline ObjectType* multiplyTransformation(const MatrixType& transformation, Transformation type = Transformation::Global) {
            setTransformation(type == Transformation::Global ?
                transformation*_transformation : _transformation*transformation);
            return static_cast<ObjectType*>(this);
        }

        /*@}*/

        /**
         * @brief Draw object
         * @param transformationMatrix      %Matrix specifying object
         *      transformation relative to the scene.
         * @param camera                    Active camera (containing
         *      projection matrix)
         *
         * Default implementation does nothing.
         */
        virtual void draw(const MatrixType& transformationMatrix, CameraType* camera);

        /** @{ @name Caching helpers
         *
         * If the object (absolute) transformation or anything depending on it
         * is used many times when drawing (such as e.g. position of light
         * object), it's good to cache these values, so they don't have to be
         * recalculated again on every request.
         *
         * If setDirty() is called on an object (or the object is transformed),
         * it and all its children are marked as dirty. If any object is
         * already dirty, it and all its children are skipped, because they
         * are already dirty too.
         *
         * If setClean() is called on an object, it and all its parents are
         * cleaned. If any object is already clean, it and all its parents are
         * skipped, because they are already clean too.
         *
         * These functions are used to manage dirty status of the object. If
         * the object doesn't cache anything, it's no need to bother about
         * them, but if does, clean() should be reimplemented and used to
         * regenerate the cache.
         */

        /**
         * @brief Whether the object is dirty
         * @return  True, if transformation of the object, any parent or camera
         *      has changed since last asking, false otherwise.
         */
        inline bool isDirty() const { return dirty; }

        /**
         * @brief Set object and all its children as dirty
         *
         * Recursively calls setDirty() on every child. If the object is
         * already marked as dirty, the function does nothing. It is usually
         * not needed to reimplement this function, only if you for example
         * need to reset some state on object which is not child of this. All
         * computations should be done in setClean().
         *
         * Reimplementations should call this function at the end, i.e.:
         * @code
         * void setDirty() {
         *     // ...
         *
         *     Object::setDirty();
         * }
         * @endcode
         */
        virtual void setDirty();

        /**
         * @brief Set object and all its parents as clean
         *
         * Recursively calls clean() on every parent which is not already
         * clean.
         */
        void setClean();

    protected:
        /**
         * @brief Clean the object
         *
         * When reimplementing, use absolute transformation passed as
         * parameter instead of absoluteTransformation(), which is not
         * efficient. The reimplementation should call this function at the
         * beginning, i.e.:
         * @code
         * void clean(const Matrix4& absoluteTransformation) {
         *     Object::clean(absoluteTransformation);
         *
         *     // ...
         * }
         * @endcode
         */
        virtual void clean(const MatrixType& absoluteTransformation);

        /*@}*/

    private:
        /* Hide base class members, as they are aliased to more meaningful names */
        using Corrade::Containers::LinkedList<ObjectType>::first;
        using Corrade::Containers::LinkedList<ObjectType>::last;
        using Corrade::Containers::LinkedList<ObjectType>::isEmpty;
        using Corrade::Containers::LinkedList<ObjectType>::insert;
        using Corrade::Containers::LinkedList<ObjectType>::cut;
        using Corrade::Containers::LinkedList<ObjectType>::move;
        using Corrade::Containers::LinkedList<ObjectType>::erase;
        using Corrade::Containers::LinkedList<ObjectType>::clear;
        using Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::list;
        using Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::previous;
        using Corrade::Containers::LinkedListItem<ObjectType, ObjectType>::next;

        MatrixType _transformation;
        bool dirty;
};

/* Implementations for inline functions with unused parameters */
template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> inline void Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::draw(const MatrixType&, CameraType*) {}
template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> inline void Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::clean(const MatrixType&) { dirty = false; }

class Camera2D;
class Camera3D;
class Object2D;
class Object3D;
template<class MatrixType, class VectorType, class ObjectType, class CameraType> class Scene;
typedef Scene<Matrix3, Vector2, Object2D, Camera2D> Scene2D;
typedef Scene<Matrix4, Vector3, Object3D, Camera3D> Scene3D;

#ifndef DOXYGEN_GENERATING_OUTPUT
/* These templates are instantiated in source file */
extern template class SCENEGRAPH_EXPORT Object<Matrix3, Vector2, Object2D, Scene2D, Camera2D>;
extern template class SCENEGRAPH_EXPORT Object<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;
#endif

/** @brief Two-dimensional object */
class SCENEGRAPH_EXPORT Object2D: public Object<Matrix3, Vector2, Object2D, Scene2D, Camera2D> {
    public:
        /** @copydoc Object::Object */
        inline Object2D(Object2D* parent = nullptr): Object(parent) {}

        /**
         * @brief Translate object
         *
         * Same as calling multiplyTransformation() with Matrix3::translation().
         */
        inline Object2D* translate(const Vector2& vec, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix3::translation(vec), type);
            return this;
        }

        /**
         * @brief Scale object
         *
         * Same as calling multiplyTransformation() with Matrix3::scaling().
         */
        inline Object2D* scale(const Vector2& vec, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix3::scaling(vec), type);
            return this;
        }

        /**
         * @brief Rotate object
         *
         * Same as calling multiplyTransformation() with Matrix3::rotation().
         */
        inline Object2D* rotate(GLfloat angle, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix3::rotation(angle), type);
            return this;
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         */
        inline Object2D* move(Object2D* under) {
            parent()->Corrade::Containers::LinkedList<Object2D>::move(this, under);
            return this;
        }
};

/** @brief Three-dimensional object */
class SCENEGRAPH_EXPORT Object3D: public Object<Matrix4, Vector3, Object3D, Scene3D, Camera3D> {
    public:
        /** @copydoc Object::Object */
        inline Object3D(Object3D* parent = nullptr): Object(parent) {}

        /**
         * @brief Translate object
         *
         * Same as calling multiplyTransformation() with Matrix4::translation().
         */
        inline Object3D* translate(const Vector3& vec, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix4::translation(vec), type);
            return this;
        }

        /**
         * @brief Scale object
         *
         * Same as calling multiplyTransformation() with Matrix4::scaling().
         */
        inline Object3D* scale(const Vector3& vec, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix4::scaling(vec), type);
            return this;
        }

        /**
         * @brief Rotate object
         *
         * Same as calling multiplyTransformation() with Matrix4::rotation().
         */
        inline Object3D* rotate(GLfloat angle, const Vector3& vec, Transformation type = Transformation::Global) {
            multiplyTransformation(Matrix4::rotation(angle, vec), type);
            return this;
        }
};

}}

#endif
