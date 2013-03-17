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

#include "Physics/LineSegment.h"
#include "Physics/Point.h"
#include "Physics/Plane.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Physics { namespace Test {

class PlaneTest: public Corrade::TestSuite::Tester, ShapeTestBase {
    public:
        PlaneTest();

        void applyTransformation();
        void collisionLine();
        void collisionLineSegment();
};

PlaneTest::PlaneTest() {
    addTests({&PlaneTest::applyTransformation,
              &PlaneTest::collisionLine,
              &PlaneTest::collisionLineSegment});
}

void PlaneTest::applyTransformation() {
    Physics::Plane plane({1.0f, 2.0f, 3.0f}, {Constants::sqrt2(), -Constants::sqrt2(), 0});

    plane.applyTransformationMatrix(Matrix4::rotation(Deg(90.0f), Vector3::xAxis()));
    CORRADE_COMPARE(plane.transformedPosition(), Vector3(1.0f, -3.0f, 2.0f));
    CORRADE_COMPARE(plane.transformedNormal(), Vector3(Constants::sqrt2(), 0, -Constants::sqrt2()));

    /* The normal should stay normalized */
    plane.applyTransformationMatrix(Matrix4::scaling({1.5f, 2.0f, 3.0f}));
    CORRADE_COMPARE(plane.transformedPosition(), Vector3(1.5f, 4.0f, 9.0f));
    CORRADE_COMPARE(plane.transformedNormal(), Vector3(Constants::sqrt2(), -Constants::sqrt2(), 0));
}

void PlaneTest::collisionLine() {
    Physics::Plane plane(Vector3(), Vector3::yAxis());
    Physics::Line3D line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    Physics::Line3D line2({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});
    Physics::Line3D line3({0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});

    randomTransformation(plane);
    randomTransformation(line);
    randomTransformation(line2);
    randomTransformation(line3);

    VERIFY_COLLIDES(plane, line);
    VERIFY_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

void PlaneTest::collisionLineSegment() {
    Physics::Plane plane(Vector3(), Vector3::yAxis());
    Physics::LineSegment3D line({0.0f, -0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Physics::LineSegment3D line2({0.0f, 0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Physics::LineSegment3D line3({0.0f, -7.0f, 0.0f}, {0.0f, -0.1f, 0.0f});

    randomTransformation(plane);
    randomTransformation(line);
    randomTransformation(line2);
    randomTransformation(line3);

    VERIFY_COLLIDES(plane, line);
    VERIFY_NOT_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::PlaneTest)
