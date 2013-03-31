#ifndef Magnum_Physics_AxisAlignedBox_h
#define Magnum_Physics_AxisAlignedBox_h
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
 * @brief Class Magnum::Physics::AxisAlignedBox, typedef Magnum::Physics::AxisAlignedBox2D, Magnum::Physics.:AxisAlignedBox3D
 */

#include "Math/Vector3.h"
#include "Physics/AbstractShape.h"
#include "Physics/Physics.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief Axis-aligned box

Unit-size means that half extents are equal to 1, equivalent to e.g. sphere
radius.
@see AxisAlignedBox2D, AxisAlignedBox3D
@todo Assert for rotation
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT AxisAlignedBox: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline explicit AxisAlignedBox(const typename DimensionTraits<dimensions>::VectorType& min, const typename DimensionTraits<dimensions>::VectorType& max): _min(min), _max(max), _transformedMin(min), _transformedMax(max) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::AxisAlignedBox;
        }

        void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) override;

        bool collides(const AbstractShape<dimensions>* other) const override;

        /** @brief Minimal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType min() const {
            return _min;
        }

        /** @brief Set minimal coordinates */
        inline void setMin(const typename DimensionTraits<dimensions>::VectorType& min) {
            _min = min;
        }

        /** @brief Maximal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType max() const { return _max; }

        /** @brief Set maximal coordinates */
        inline void setMax(const typename DimensionTraits<dimensions>::VectorType& max) {
            _max = max;
        }

        /** @brief Transformed minimal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType transformedMin() const {
            return _transformedMin;
        }

        /** @brief Transformed maximal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType transformedMax() const {
            return _transformedMax;
        }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

    private:
        typename DimensionTraits<dimensions>::VectorType _min, _max,
            _transformedMin, _transformedMax;
};

/** @brief Two-dimensional axis-aligned box */
typedef AxisAlignedBox<2> AxisAlignedBox2D;

/** @brief Three-dimensional axis-aligned box */
typedef AxisAlignedBox<3> AxisAlignedBox3D;

/** @collisionoperator{Point,AxisAlignedBox} */
template<UnsignedInt dimensions> inline bool operator%(const Point<dimensions>& a, const AxisAlignedBox<dimensions>& b) { return b % a; }

}}

#endif
