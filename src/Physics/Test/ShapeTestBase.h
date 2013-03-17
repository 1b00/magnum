#ifndef Magnum_Physics_Test_ShapeTestBase_h
#define Magnum_Physics_Test_ShapeTestBase_h
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

#include <TestSuite/Tester.h>

#include "Math/Matrix4.h"
#include "Magnum.h"

namespace Magnum { namespace Physics { namespace Test {

class ShapeTestBase {
    protected:
        template<class T> void randomTransformation(T& shape) {
            shape.applyTransformationMatrix(Matrix4::translation(Vector3(7.0f, 8.0f, -9.0f)));
        }
};

#define VERIFY_COLLIDES(a, b)                                               \
    CORRADE_VERIFY(a % b);                                                  \
    CORRADE_VERIFY(b % a);                                                  \
    CORRADE_VERIFY(a.collides(&b));                                         \
    CORRADE_VERIFY(b.collides(&a));

#define VERIFY_NOT_COLLIDES(a, b)                                           \
    CORRADE_VERIFY(!(a % b));                                               \
    CORRADE_VERIFY(!(b % a));                                               \
    CORRADE_VERIFY(!(a.collides(&b)));                                      \
    CORRADE_VERIFY(!(b.collides(&a)));

}}}

#endif
