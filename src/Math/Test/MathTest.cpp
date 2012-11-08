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

#include "MathTest.h"

#include "Math.h"

using namespace std;

CORRADE_TEST_MAIN(Magnum::Math::Test::MathTest)

namespace Magnum { namespace Math { namespace Test {

MathTest::MathTest() {
    addTests(&MathTest::normalize,
             &MathTest::denormalize,
             &MathTest::clamp,
             &MathTest::pow,
             &MathTest::log);
}

void MathTest::normalize() {
    /* Range for signed and unsigned */
    CORRADE_COMPARE((Math::normalize<float, int8_t>(-128)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, int8_t>(127)), 1.0f);
    CORRADE_COMPARE((Math::normalize<float, uint8_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, uint8_t>(255)), 1.0f);

    /* Between */
    CORRADE_COMPARE((Math::normalize<float, int16_t>(16384)), 0.750011f);
    CORRADE_COMPARE((Math::normalize<float, int16_t>(-16384)), 0.250004f);

    /* Test overflow for large types */
    CORRADE_COMPARE((Math::normalize<float, int32_t>(numeric_limits<int32_t>::min())), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, int32_t>(numeric_limits<int32_t>::max())), 1.0f);
    CORRADE_COMPARE((Math::normalize<float, uint32_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, uint32_t>(numeric_limits<uint32_t>::max())), 1.0f);

    CORRADE_COMPARE((Math::normalize<double, int64_t>(numeric_limits<int64_t>::min())), 0.0);
    CORRADE_COMPARE((Math::normalize<double, int64_t>(numeric_limits<int64_t>::max())), 1.0);
    CORRADE_COMPARE((Math::normalize<double, uint64_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<double, uint64_t>(numeric_limits<uint64_t>::max())), 1.0);
}

void MathTest::denormalize() {
    /* Range for signed and unsigned */
    CORRADE_COMPARE(Math::denormalize<int8_t>(0.0f), -128);
    CORRADE_COMPARE(Math::denormalize<int8_t>(1.0f), 127);
    CORRADE_COMPARE(Math::denormalize<uint8_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<uint8_t>(1.0f), 255);

    /* Between */
    CORRADE_COMPARE(Math::denormalize<int16_t>(0.33f), -11141);
    CORRADE_COMPARE(Math::denormalize<int16_t>(0.66f), 10485);

    /* Test overflow for large types */
    CORRADE_COMPARE(Math::denormalize<int32_t>(0.0f), numeric_limits<int32_t>::min());
    CORRADE_COMPARE(Math::denormalize<uint32_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<int64_t>(0.0), numeric_limits<int64_t>::min());
    CORRADE_COMPARE(Math::denormalize<uint64_t>(0.0), 0);

    CORRADE_COMPARE(Math::denormalize<int32_t>(1.0), numeric_limits<int32_t>::max());
    CORRADE_COMPARE(Math::denormalize<uint32_t>(1.0), numeric_limits<uint32_t>::max());

//     {
//         CORRADE_EXPECT_FAIL("Denormalize doesn't work for large types well");
//         CORRADE_COMPARE((Math::denormalize<long long, long double>(1.0)), numeric_limits<long long>::max());
//         CORRADE_COMPARE((Math::denormalize<unsigned long long, long double>(1.0)), numeric_limits<unsigned long long>::max());
//     }
}

void MathTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);
}

void MathTest::pow() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);
}

void MathTest::log() {
    CORRADE_COMPARE(Math::log(2, 256), 8ul);
    CORRADE_COMPARE(Math::log(256, 2), 0ul);
}

}}}
