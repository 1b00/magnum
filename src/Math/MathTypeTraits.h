#ifndef Magnum_Math_MathTypeTraits_h
#define Magnum_Math_MathTypeTraits_h
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
 * @brief Class Magnum::Math::MathTypeTraits
 */

#include <cstddef>
#include <cmath>

#include "magnumCompatibility.h"

/** @brief Precision when testing floats for equality */
#ifndef FLOAT_EQUALITY_PRECISION
#define FLOAT_EQUALITY_PRECISION 1.0e-6
#endif

/** @brief Precision when testing doubles for equality */
#ifndef DOUBLE_EQUALITY_PRECISION
#define DOUBLE_EQUALITY_PRECISION 1.0e-12
#endif

namespace Magnum { namespace Math {

/**
@brief Traits class for numeric types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.

This class and class methods are specialized only for types where it makes
sense, it has empty implementation for unknown types or types which don't
support given feature, thus forcing the compilation stop with an error.

@internal The following values are implemented as inline functions, not as
    static const variables, because the compiler will inline the return values
    instead of referencing to static data and unlike static const variables
    the functions can be overloaded, deleted and hidden.
*/
template<class T> struct MathTypeTraits {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Corresponding numeric type large at least as `int`
     *
     * Usable e.g. to prevent conversion of `char` to characters when printing
     * numeric types to output.
     */
    typedef U NumericType;

    /**
     * @brief Corresponding floating-point type for normalization
     *
     * If the type is not already floating-point, defines smallest larger
     * floating-point type.
     */
    typedef U FloatingPointType;

    /**
     * @brief Epsilon value for fuzzy compare
     *
     * Returns minimal difference between numbers to be considered
     * inequal. Returns 1 for integer types and reasonably small value for
     * floating-point types.
     */
    inline constexpr static T epsilon();

    /**
     * @brief Fuzzy compare
     *
     * Uses equality for integer types and fuzzy compare for floating-point
     * types (using @ref epsilon value).
     */
    static bool equals(T a, T b);
    #endif
};

/** @bug Infinity comparison! */

/**
 * @todo Implement better fuzzy comparison algorithm, like at
 * http://floating-point-gui.de/errors/comparison/ or
 * http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class T> struct MathTypeTraitsIntegral {
    inline constexpr static T epsilon() { return 1; }

    inline constexpr static bool equals(T a, T b) {
        return a == b;
    }
};

template<class T> struct MathTypeTraitsFloatingPoint {
    inline static bool equals(T a, T b) {
        return std::abs(a - b) < MathTypeTraits<T>::epsilon();
    }
};

template<size_t> struct MathTypeTraitsLong {};

template<> struct MathTypeTraitsLong<sizeof(int)> {
    typedef unsigned int UnsignedType;
    typedef int Type;
};

template<> struct MathTypeTraitsLong<sizeof(long long)> {
    typedef unsigned long long UnsignedType;
    typedef long long Type;
};

}

template<> struct MathTypeTraits<unsigned char>: public Implementation::MathTypeTraitsIntegral<unsigned char> {
    typedef unsigned int NumericType;
    typedef float FloatingPointType;
};
template<> struct MathTypeTraits<char>: public Implementation::MathTypeTraitsIntegral<char> {
    typedef int NumericType;
    typedef float FloatingPointType;
};

template<> struct MathTypeTraits<unsigned short>: public Implementation::MathTypeTraitsIntegral<unsigned short> {
    typedef unsigned int NumericType;
    typedef float FloatingPointType;
};
template<> struct MathTypeTraits<short>: public Implementation::MathTypeTraitsIntegral<short> {
    typedef int NumericType;
    typedef float FloatingPointType;
};

template<> struct MathTypeTraits<unsigned int>: public Implementation::MathTypeTraitsIntegral<unsigned int> {
    typedef unsigned int NumericType;
    typedef double FloatingPointType;
};
template<> struct MathTypeTraits<int>: public Implementation::MathTypeTraitsIntegral<int> {
    typedef int NumericType;
    typedef double FloatingPointType;
};

template<> struct MathTypeTraits<unsigned long long>: public Implementation::MathTypeTraitsIntegral<unsigned long long> {
    typedef unsigned long long NumericType;
    typedef long double FloatingPointType;
};
template<> struct MathTypeTraits<long long>: public Implementation::MathTypeTraitsIntegral<long long> {
    typedef long long NumericType;
    typedef long double FloatingPointType;
};

/* long is 32 bits somewhere and 64 bits elsewhere */
template<> struct MathTypeTraits<long unsigned int>: public Implementation::MathTypeTraitsIntegral<typename Implementation::MathTypeTraitsLong<sizeof(long unsigned int)>::Type> {};
template<> struct MathTypeTraits<long int>: public Implementation::MathTypeTraitsIntegral<typename Implementation::MathTypeTraitsLong<sizeof(long int)>::Type> {};

template<> struct MathTypeTraits<float>: public Implementation::MathTypeTraitsFloatingPoint<float> {
    typedef float NumericType;
    typedef float FloatingPointType;

    inline constexpr static float epsilon() { return FLOAT_EQUALITY_PRECISION; }
};
template<> struct MathTypeTraits<double>: public Implementation::MathTypeTraitsFloatingPoint<double> {
    typedef float NumericType;
    typedef double FloatingPointType;

    inline constexpr static double epsilon() { return DOUBLE_EQUALITY_PRECISION; }
};
#endif

}}

#endif
