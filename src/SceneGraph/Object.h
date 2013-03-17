#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class Magnum::SceneGraph::Object
 */

#include <Containers/EnumSet.h>

#include "AbstractFeature.h"
#include "AbstractObject.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    enum class ObjectFlag: UnsignedByte {
        Dirty = 1 << 0,
        Visited = 1 << 1,
        Joint = 1 << 2
    };

    typedef Corrade::Containers::EnumSet<ObjectFlag, UnsignedByte> ObjectFlags;

    CORRADE_ENUMSET_OPERATORS(ObjectFlags)
}
#endif

/**
@brief %Object

Base of scene graph. Contains specific transformation implementation, takes
care of parent/children relationship and contains features. See @ref scenegraph
for introduction.

Common usage is to typedef Object with desired transformation type to save
unnecessary typing later, along with Scene and possibly other types, e.g.:
@code
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;
@endcode

Uses Corrade::Containers::LinkedList for parent/children relationship.
Traversing through the list is done like in the following code. It is also
possible to go in reverse order using lastChild() and previousSibling().
@code
for(Object* child = o->firstChild(); child; child = child->nextSibling()) {
    // ...
}
@endcode

@section Object-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations (e.g. using Double type or special transformation
class) you have to use Object.hpp implementation file to avoid linker errors.
See @ref compilation-speedup-hpp for more information.

 - @ref DualComplexTransformation "Object<DualComplexTransformation<Float>>"
 - @ref DualQuaternionTransformation "Object<DualQuaternionTransformation<Float>>"
 - @ref MatrixTransformation2D "Object<MatrixTransformation2D<Float>>"
 - @ref MatrixTransformation3D "Object<MatrixTransformation3D<Float>>"
 - @ref RigidMatrixTransformation2D "Object<RigidMatrixTransformation2D<Float>>"
 - @ref RigidMatrixTransformation3D "Object<RigidMatrixTransformation3D<Float>>"

@see Scene, AbstractFeature, AbstractTransformation, DebugTools::ObjectRenderer
*/
template<class Transformation> class MAGNUM_SCENEGRAPH_EXPORT Object: public AbstractObject<Transformation::Dimensions, typename Transformation::Type>, public Transformation
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , private Corrade::Containers::LinkedList<Object<Transformation>>, private Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>
    #endif
{
    friend class Corrade::Containers::LinkedList<Object<Transformation>>;
    friend class Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    Object(const Object<Transformation>& other) = delete;
    Object(Object<Transformation>&& other) = delete;
    Object<Transformation>& operator=(const Object<Transformation>& other) = delete;
    Object<Transformation>& operator=(Object<Transformation>&& other) = delete;
    #endif

    public:
        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @see setClean(), AbstractObject::setClean()
         */
        /* `objects` passed by copy intentionally (to avoid copy internally) */
        static void setClean(std::vector<Object<Transformation>*> objects);

        /**
         * @brief Constructor
         * @param parent    Parent object
         */
        inline explicit Object(Object<Transformation>* parent = nullptr): counter(0xFFFFu), flags(Flag::Dirty) {
            setParent(parent);
        }

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        inline virtual ~Object() {}

        /**
         * @{ @name Scene hierarchy
         *
         * See @ref scenegraph-hierarchy for more information.
         */

        /** @brief Whether this object is scene */
        virtual inline bool isScene() const { return false; }

        /**
         * @brief %Scene
         * @return %Scene or `nullptr`, if the object is not part of any scene.
         */
        Scene<Transformation>* scene();

        /** @overload */
        const Scene<Transformation>* scene() const;

        /** @brief Parent object or `nullptr`, if this is root object */
        inline Object<Transformation>* parent() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @overload */
        inline const Object<Transformation>* parent() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @brief Previous sibling object or `nullptr`, if this is first object */
        inline Object<Transformation>* previousSibling() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @overload */
        inline const Object<Transformation>* previousSibling() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @brief Next sibling object or `nullptr`, if this is last object */
        inline Object<Transformation>* nextSibling() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @overload */
        inline const Object<Transformation>* nextSibling() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @brief Whether this object has children */
        inline bool hasChildren() const {
            return !Corrade::Containers::LinkedList<Object<Transformation>>::isEmpty();
        }

        /** @brief First child object or `nullptr`, if this object has no children */
        inline Object<Transformation>* firstChild() {
            return Corrade::Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @overload */
        inline const Object<Transformation>* firstChild() const {
            return Corrade::Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @brief Last child object or `nullptr`, if this object has no children */
        inline Object<Transformation>* lastChild() {
            return Corrade::Containers::LinkedList<Object<Transformation>>::last();
        }

        /** @overload */
        inline const Object<Transformation>* lastChild() const {
            return Corrade::Containers::LinkedList<Object<Transformation>>::last();
        }

        /**
         * @brief Set parent object
         * @return Pointer to self (for method chaining)
         */
        Object<Transformation>* setParent(Object<Transformation>* parent);

        /*@}*/

        /** @{ @name Object transformation */

        inline typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType transformationMatrix() const override {
            return Transformation::toMatrix(Transformation::transformation());
        }

        inline typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType absoluteTransformationMatrix() const override {
            return Transformation::toMatrix(absoluteTransformation());
        }

        /**
         * @brief Transformation relative to root object
         *
         * @see absoluteTransformationMatrix()
         */
        typename Transformation::DataType absoluteTransformation() const;

        /**
         * @brief Transformations of given group of objects relative to this object
         *
         * All transformations can be premultiplied with @p initialTransformation,
         * if specified.
         * @see AbstractObject::transformationMatrices()
         */
        /* `objects` passed by copy intentionally (to allow move from
           transformationMatrices() and avoid copy in the function itself) */
        std::vector<typename Transformation::DataType> transformations(std::vector<Object<Transformation>*> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;

        /*@}*/

        inline bool isDirty() const override { return !!(flags & Flag::Dirty); }
        void setDirty() override;
        void setClean() override;

    private:
        /* GCC 4.4 doesn't support lambda functions */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct DirtyCheck {
            inline bool operator()(Object<Transformation>* o) const { return !o->isDirty(); }
        };
        #endif

        Object<Transformation>* sceneObject() override;
        const Object<Transformation>* sceneObject() const override;

        std::vector<typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType> transformationMatrices(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects, const typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType& initialTransformationMatrix = (typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType())) const override;

        typename Transformation::DataType MAGNUM_SCENEGRAPH_LOCAL computeJointTransformation(const std::vector<Object<Transformation>*>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& initialTransformation) const;

        void setClean(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects) const override;

        void MAGNUM_SCENEGRAPH_LOCAL setClean(const typename Transformation::DataType& absoluteTransformation);

        typedef Implementation::ObjectFlag Flag;
        typedef Implementation::ObjectFlags Flags;
        UnsignedShort counter;
        Flags flags;
};

}}

#endif
