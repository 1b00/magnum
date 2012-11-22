#ifndef Magnum_Math_Vector2_h
#define Magnum_Math_Vector2_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 2 for more details.
*/

/** @file
 * @brief Class Magnum::Math::Vector2
 */

#include "Vector.h"

namespace Magnum { namespace Math {

/**
@brief Two-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see Magnum::Vector2
@configurationvalueref{Magnum::Math::Vector2}
*/
template<class T> class Vector2: public Vector<2, T> {
    public:
        /**
         * @brief %Vector in direction of X axis (right)
         *
         * Usable for translation in given axis, for example:
         * @code
         * Matrix3::translation(Vector2::xAxis(5.0f)); // same as Matrix3::translation({5.0f, 0.0f});
         * @endcode
         * @see yAxis(), xScale(), Matrix3::right()
         */
        inline constexpr static Vector2<T> xAxis(T length = T(1)) { return Vector2<T>(length, T()); }

        /**
         * @brief %Vector in direction of Y axis (up)
         *
         * See xAxis() for more information.
         * @see yScale(), Matrix3::up()
         */
        inline constexpr static Vector2<T> yAxis(T length = T(1)) { return Vector2<T>(T(), length); }

        /**
         * @brief Scaling vector in direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         * @code
         * Matrix3::scaling(Vector2::xScale(-2.0f)); // same as Matrix3::scaling({-2.0f, 1.0f});
         * @endcode
         * @see yScale(), xAxis()
         */
        inline constexpr static Vector2<T> xScale(T scale) { return Vector2<T>(scale, T(1)); }

        /**
         * @brief Scaling vector in direction of Y axis (height)
         *
         * See xScale() for more information.
         * @see yAxis()
         */
        inline constexpr static Vector2<T> yScale(T scale) { return Vector2<T>(T(1), scale); }

        /** @copydoc Vector::Vector() */
        inline constexpr Vector2() {}

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector2(T value): Vector<2, T>(value, value) {}

        /** @brief Copy constructor */
        inline constexpr Vector2(const RectangularMatrix<1, 2, T>& other): Vector<2, T>(other) {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         */
        inline constexpr Vector2(T x, T y): Vector<2, T>(x, y) {}

        inline T& x() { return (*this)[0]; }                /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; } /**< @overload */
        inline T& y() { return (*this)[1]; }                /**< @brief Y component */
        inline constexpr T y() const { return (*this)[1]; } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector2, 2)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, 2, Vector2<T>)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Vector2, 2)

/** @debugoperator{Magnum::Math::Vector2} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector2<T>& value) {
    return debug << static_cast<const Vector<2, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector2} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector2<T>>: public ConfigurationValue<Magnum::Math::Vector<2, T>> {};
}}

#endif
