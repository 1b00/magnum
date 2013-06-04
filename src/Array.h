#ifndef Magnum_Array_h
#define Magnum_Array_h
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

/** @file /Array.h
 * @brief Class Magnum::Array, Magnum::Array1D, Magnum::Array2D, Magnum::Array3D
 */

#include <type_traits>
#include <Utility/Debug.h>

#include "Magnum.h"

namespace Magnum {

/**
@brief %Array
@tparam dimensions  Dimension count
@tparam T           Data type

Similar to Math::Vector, but more suitable for storing enum values which don't
need any math operations and fuzzy comparison (e.g. enum values). Unlike
Math::Vector this class has non-explicit constructor from one value.
@see Array1D, Array2D, Array3D
*/
template<UnsignedInt dimensions, class T> class Array {
    public:
        typedef T Type;                                     /**< @brief Data type */
        const static UnsignedInt Dimensions = dimensions;  /**< @brief Dimension count */

        /**
         * @brief Default constructor
         *
         * Sets all components to their default-constructed values
         */
        constexpr /*implicit*/ Array(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first     First value
         * @param next      Next values
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> constexpr /*implicit*/ Array(T first, T second, U... next): _data{first, second, next...} {
            static_assert(sizeof...(next)+2 == dimensions, "Improper number of arguments passed to Array constructor");
        }
        template<class U = T> constexpr /*implicit*/ Array(typename std::enable_if<std::is_same<T, U>::value && dimensions == 1, U>::type first): _data{first} {}
        #else
        template<class ...U> constexpr /*implicit*/ Array(T first, U... next);
        #endif

        /**
         * @brief Constructor
         * @param value Value for all fields
         */
        template<class U, class = typename std::enable_if<std::is_same<T, U>::value && dimensions != 1, U>::type> /*implicit*/ Array(U value) {
            for(UnsignedInt i = 0; i != dimensions; ++i)
                _data[i] = value;
        }

        /** @brief Equality */
        bool operator==(const Array<dimensions, T>& other) const {
            for(UnsignedInt i = 0; i != dimensions; ++i)
                if(_data[i] != other._data[i]) return false;
            return true;
        }

        /** @brief Non-equality */
        bool operator!=(const Array<dimensions, T>& other) const {
            return !operator==(other);
        }

        /** @brief Value at given position */
        T& operator[](UnsignedInt pos) { return _data[pos]; }
        constexpr T operator[](UnsignedInt pos) const { return _data[pos]; } /**< @overload */

        /**
         * @brief Raw data
         * @return One-dimensional array of `dimensions` length
         */
        T* data() { return _data; }
        constexpr const T* data() const { return _data; } /**< @overload */

    private:
        T _data[dimensions];
};

/**
@brief One-dimensional array
@tparam T           Data type
*/
template<class T> class Array1D: public Array<1, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array1D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         */
        constexpr /*implicit*/ Array1D(T x): Array<1, T>(x) {}

        /** @brief Copy constructor */
        constexpr Array1D(const Array<1, T>& other): Array<1, T>(other) {}

        T& x() { return (*this)[0]; }                  /**< @brief X component */
        constexpr T x() const { return (*this)[0]; }   /**< @overload */
};

/**
@brief Two-dimensional array
@tparam T           Data type
*/
template<class T> class Array2D: public Array<2, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array2D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         */
        constexpr /*implicit*/ Array2D(T x, T y): Array<2, T>(x, y) {}

        /** @copydoc Array::Array(U) */
        constexpr /*implicit*/ Array2D(T value): Array<2, T>(value, value) {}

        /** @brief Copy constructor */
        constexpr Array2D(const Array<2, T>& other): Array<2, T>(other) {}

        T& x() { return (*this)[0]; }                  /**< @brief X component */
        constexpr T x() const { return (*this)[0]; }   /**< @overload */
        T& y() { return (*this)[1]; }                  /**< @brief Y component */
        constexpr T y() const { return (*this)[1]; }   /**< @overload */
};

/**
@brief Three-dimensional array
@tparam T           Data type
*/
template<class T> class Array3D: public Array<3, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array3D() {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         * @param z     Z component
         */
        constexpr /*implicit*/ Array3D(T x, T y, T z): Array<3, T>(x, y, z) {}

        /** @copydoc Array::Array(U) */
        constexpr /*implicit*/ Array3D(T value): Array<3, T>(value, value, value) {}

        /** @brief Copy constructor */
        constexpr Array3D(const Array<3, T>& other): Array<3, T>(other) {}

        T& x() { return (*this)[0]; }                  /**< @brief X component */
        constexpr T x() const { return (*this)[0]; }   /**< @overload */
        T& y() { return (*this)[1]; }                  /**< @brief Y component */
        constexpr T y() const { return (*this)[1]; }   /**< @overload */
        T& z() { return (*this)[2]; }                  /**< @brief Z component */
        constexpr T z() const { return (*this)[2]; }   /**< @overload */
};

/** @debugoperator{Magnum::Array} */
template<UnsignedInt dimensions, class T> Debug operator<<(Debug debug, const Array<dimensions, T>& value) {
    debug << "Array(";
    debug.setFlag(Debug::SpaceAfterEachValue, false);
    for(UnsignedInt i = 0; i != dimensions; ++i) {
        if(i != 0) debug << ", ";
        debug << value[i];
    }
    debug << ")";
    debug.setFlag(Debug::SpaceAfterEachValue, true);
    return debug;
}

/** @debugoperator{Magnum::Array1D} */
template<class T> inline Debug operator<<(Debug debug, const Array1D<T>& value) {
    return debug << static_cast<const Array<1, T>&>(value);
}

/** @debugoperator{Magnum::Array2D} */
template<class T> inline Debug operator<<(Debug debug, const Array2D<T>& value) {
    return debug << static_cast<const Array<2, T>&>(value);
}

/** @debugoperator{Magnum::Array3D} */
template<class T> inline Debug operator<<(Debug debug, const Array3D<T>& value) {
    return debug << static_cast<const Array<3, T>&>(value);
}

}

#endif
