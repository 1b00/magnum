/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/ImageReference.h"

namespace Magnum { namespace Test {

class ImageReferenceTest: public TestSuite::Tester {
    public:
        explicit ImageReferenceTest();

        void construct();
        void setData();
};

ImageReferenceTest::ImageReferenceTest() {
    addTests({&ImageReferenceTest::construct,
              &ImageReferenceTest::setData});
}

void ImageReferenceTest::construct() {
    const unsigned char data[3] = {};
    ImageReference2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageReferenceTest::setData() {
    const unsigned char data[3] = {};
    ImageReference2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    const unsigned char data2[8] = {};
    a.setData(data2);

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), reinterpret_cast<const unsigned char*>(data2));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageReferenceTest)
