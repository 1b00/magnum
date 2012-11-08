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

#include "Vector4Test.h"

#include <sstream>

#include "Vector4.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector4Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector4<float> Vector4;
typedef Math::Vector3<float> Vector3;
typedef Math::Vector2<float> Vector2;

Vector4Test::Vector4Test() {
    addTests(&Vector4Test::construct,
             &Vector4Test::threeComponent,
             &Vector4Test::twoComponent,
             &Vector4Test::debug,
             &Vector4Test::configuration);
}

void Vector4Test::construct() {
    CORRADE_COMPARE(Vector4(), Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(Vector4(1, 2, 3, 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
    CORRADE_COMPARE(Vector4(Vector<3, float>(1.0f, 2.0f, 3.0f), 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
}

void Vector4Test::threeComponent() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).xyz(), Vector3(1.0f, 2.0f, 3.0f));
}

void Vector4Test::twoComponent() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).xy(), Vector2(1.0f, 2.0f));
}

void Vector4Test::debug() {
    ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");
}

void Vector4Test::configuration() {
    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    string value("3 3.125 9 9.55");
    CORRADE_COMPARE(ConfigurationValue<Vector4>::toString(vec), value);
    CORRADE_COMPARE(ConfigurationValue<Vector4>::fromString(value), vec);
}

}}}
