#ifndef Magnum_Swizzle_h
#define Magnum_Swizzle_h
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

/** @file /Swizzle.h
 * @brief Function Magnum::swizzle()
 */

#include "Math/Swizzle.h"
#include "Color.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Math { namespace Implementation {
    template<std::size_t size> struct Component<size, 'r'>: public ComponentAtPosition<size, 0> {};
    template<std::size_t size> struct Component<size, 'g'>: public ComponentAtPosition<size, 1> {};
    template<std::size_t size> struct Component<size, 'b'>: public ComponentAtPosition<size, 2> {};
    template<std::size_t size> struct Component<size, 'a'>: public ComponentAtPosition<size, 3> {};
}}

namespace Implementation {
    template<std::size_t size, class T> struct TypeForSize {
        typedef Math::Vector<size, typename T::Type> Type;
    };
    template<class T> struct TypeForSize<2, T> { typedef Math::Vector2<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };
    template<class T> struct TypeForSize<4, T> { typedef Math::Vector4<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };
}
#endif

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4i original(-1, 2, 3, 4);

auto vec = swizzle<'a', '1', '0', 'r', 'g', 'b'>(original);
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a` for addressing
components or letters `0` and `1` for zero and one. Count of elements is
unlimited, but must be at least one. If the resulting vector is two, three or
four-component, corresponding Math::Vector2, Math::Vector3, Math::Vector4,
Color3 or Color4 specialization is returned.

@see @ref matrix-vector-component-access, Math::swizzle(), Vector4::xyz(),
    Color4::rgb(), Vector4::xy(), Vector3::xy()
*/
template<char ...components, class T> inline constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return {Math::Implementation::Component<T::Size, components>::value(vector)...};
}

}

#endif
