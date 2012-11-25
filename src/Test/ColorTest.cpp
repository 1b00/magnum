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

#include "ColorTest.h"

#include <Utility/Configuration.h>

#include "Color.h"

using namespace std;

CORRADE_TEST_MAIN(Magnum::Test::ColorTest)

using namespace Corrade::Utility;

namespace Magnum { namespace Test {

typedef Magnum::Color3<uint8_t> Color3;
typedef Magnum::Color4<uint8_t> Color4;
typedef Magnum::Color3<float> Color3f;
typedef Magnum::Color4<float> Color4f;

ColorTest::ColorTest() {
    addTests(&ColorTest::fromDenormalized,
             &ColorTest::fromNormalized,

             &ColorTest::fromHue,
             &ColorTest::fromSaturation,
             &ColorTest::fromValue,

             &ColorTest::hue,
             &ColorTest::saturation,
             &ColorTest::value,

             &ColorTest::hsv,
             &ColorTest::hsvOverflow,
             &ColorTest::hsvAlpha,

             &ColorTest::debug,
             &ColorTest::configuration);
}

void ColorTest::fromDenormalized() {
    CORRADE_COMPARE(Color3f::fromDenormalized(Color3(75, 115, 224)), Color3f(0.294118, 0.45098, 0.878431));
}

void ColorTest::fromNormalized() {
    CORRADE_COMPARE(Color3::fromNormalized(Color3f(0.294118, 0.45098, 0.878431)), Color3(75, 115, 224));
}

void ColorTest::fromHue() {
    CORRADE_COMPARE(Color3::fromHSV(27.0f, 1.0f, 1.0f), Color3(255, 115, 0));
    CORRADE_COMPARE(Color3::fromHSV(86.0f, 1.0f, 1.0f), Color3(145, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(134.0f, 1.0f, 1.0f), Color3(0, 255, 60));
    CORRADE_COMPARE(Color3::fromHSV(191.0f, 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(269.0f, 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(317.0f, 1.0f, 1.0f), Color3(255, 0, 183));
}

void ColorTest::hue() {
    CORRADE_COMPARE(Color3(255, 115, 0).hue(), 27.058824f);
    CORRADE_COMPARE(Color3(145, 255, 0).hue(), 85.882353f);
    CORRADE_COMPARE(Color3(0, 255, 60).hue(), 134.11765f);
    CORRADE_COMPARE(Color3(0, 208, 255).hue(), 191.05882f);
    CORRADE_COMPARE(Color3(123, 0, 255).hue(), 268.94117f);
    CORRADE_COMPARE(Color3(255, 0, 183).hue(), 316.94117f);
}

void ColorTest::fromSaturation() {
    CORRADE_COMPARE(Color3::fromHSV(0.0f, 0.702f, 1.0f), Color3(255, 76, 76));
}

void ColorTest::saturation() {
    CORRADE_COMPARE(Color3(255, 76, 76).saturation(), 0.701961f);
    CORRADE_COMPARE(Color3().saturation(), 0.0f);
}

void ColorTest::fromValue() {
    CORRADE_COMPARE(Color3::fromHSV(0.0f, 1.0f, 0.522f), Color3(133, 0, 0));
}

void ColorTest::value() {
    CORRADE_COMPARE(Color3(133, 0, 0).value(), 0.521569f);
}

void ColorTest::hsv() {
    CORRADE_COMPARE(Color3::fromHSV(230.0f, 0.749f, 0.427f), Color3(27, 41, 109));

    float hue, saturation, value;
    tie(hue, saturation, value) = Color3(27, 41, 109).toHSV();
    CORRADE_COMPARE(hue, 229.756106f);
    CORRADE_COMPARE(saturation, 0.752294f);
    CORRADE_COMPARE(value, 0.427451f);
}

void ColorTest::hsvOverflow() {
    CORRADE_COMPARE(Color3::fromHSV(27.0f-360.0f, 1.0f, 1.0f), Color3(255, 115, 0));
    CORRADE_COMPARE(Color3::fromHSV(86.0f-360.0f, 1.0f, 1.0f), Color3(145, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(134.0f-360.0f, 1.0f, 1.0f), Color3(0, 255, 60));
    CORRADE_COMPARE(Color3::fromHSV(191.0f-360.0f, 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(269.0f-360.0f, 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(317.0f-360.0f, 1.0f, 1.0f), Color3(255, 0, 183));

    CORRADE_COMPARE(Color3::fromHSV(360.0f+27.0f, 1.0f, 1.0f), Color3(255, 115, 0));
    CORRADE_COMPARE(Color3::fromHSV(360.0f+86.0f, 1.0f, 1.0f), Color3(145, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(360.0f+134.0f, 1.0f, 1.0f), Color3(0, 255, 60));
    CORRADE_COMPARE(Color3::fromHSV(360.0f+191.0f, 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(360.0f+269.0f, 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(360.0f+317.0f, 1.0f, 1.0f), Color3(255, 0, 183));
}

void ColorTest::hsvAlpha() {
    CORRADE_COMPARE(Color4::fromHSV(make_tuple(230.0f, 0.749f, 0.427f), 23), Color4(27, 41, 109, 23));
    CORRADE_COMPARE(Color4::fromHSV(230.0f, 0.749f, 0.427f, 23), Color4(27, 41, 109, 23));
}

void ColorTest::debug() {
    ostringstream o;
    Debug(&o) << Color3f(0.5f, 0.75f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 1)\n");

    o.str("");
    Debug(&o) << Color4f(0.5f, 0.75f, 0.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 0, 1)\n");
}

void ColorTest::configuration() {
    Configuration c;

    Color3f color3(0.5f, 0.75f, 1.0f);
    string value3("0.5 0.75 1");

    c.setValue("color3", color3);
    CORRADE_COMPARE(c.value<std::string>("color3"), value3);
    CORRADE_COMPARE(c.value<Color3f>("color3"), color3);

    Color4f color4(0.5f, 0.75f, 0.0f, 1.0f);
    string value4("0.5 0.75 0 1");

    c.setValue("color4", color4);
    CORRADE_COMPARE(c.value<std::string>("color4"), value4);
    CORRADE_COMPARE(c.value<Color4f>("color4"), color4);
}

}}
