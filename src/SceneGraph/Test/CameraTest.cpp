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

#include "CameraTest.h"

#include "Math/Constants.h"
#include "SceneGraph/Camera.h"

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::CameraTest)

namespace Magnum { namespace SceneGraph { namespace Test {

CameraTest::CameraTest() {
    addTests(&CameraTest::fixAspectRatio,
             &CameraTest::defaultProjection2D,
             &CameraTest::defaultProjection3D,
             &CameraTest::projection2D,
             &CameraTest::orthographic,
             &CameraTest::perspective,
             &CameraTest::projectionSizeViewport);
}

void CameraTest::fixAspectRatio() {
    Vector2 projectionScale(0.5f, 1.0f/3.0f);
    Math::Vector2<GLsizei> size(400, 300);

    /* Division by zero */
    Vector2 projectionScaleZeroY(0.5f, 0.0f);
    Vector2 projectionScaleZeroX(0.0f, 0.5f);
    Math::Vector2<GLsizei> sizeZeroY(400, 0);
    Math::Vector2<GLsizei> sizeZeroX(0, 300);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionScaleZeroX, size), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionScaleZeroY, size), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionScale, sizeZeroY), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, projectionScale, sizeZeroX), Matrix4());

    /* Not preserved */
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::NotPreserved, projectionScale, size), Matrix4());

    /* Clip */
    Matrix4 expectedClip(1.0f, 0.0f,      0.0f, 0.0f,
                         0.0f, 4.0f/3.0f, 0.0f, 0.0f,
                         0.0f, 0.0f,      1.0f, 0.0f,
                         0.0f, 0.0f,      0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, Vector2(0.5f), size), expectedClip);
    Matrix4 expectedClipRectangle(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 2.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionScale, size), expectedClipRectangle);

    /* Extend */
    Matrix4 expectedExtend(3.0f/4.0f, 0.0f, 0.0f, 0.0f,
                           0.0f,      1.0f, 0.0f, 0.0f,
                           0.0f,      0.0f, 1.0f, 0.0f,
                           0.0f,      0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, Vector2(0.5f), size), expectedExtend);
    Matrix4 expectedExtendRectangle(0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, projectionScale, size), expectedExtendRectangle);
}

void CameraTest::defaultProjection2D() {
    Camera2D camera;
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix3());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::defaultProjection3D() {
    Camera3D camera;
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix4());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::projection2D() {
    Vector2 projectionSize(4.0f, 3.0f);
    Camera2D camera;
    camera.setProjection(projectionSize);

    Matrix3 a(2.0f/4.0f,    0.0f,       0.0f,
              0.0f,         2.0f/3.0f,  0.0f,
              0.0f,         0.0f,       1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);
}

void CameraTest::orthographic() {
    Vector2 projectionSize(5);
    Camera3D camera;
    camera.setOrthographic(projectionSize, 1, 9);

    Matrix4 a(0.4f,   0.0f,   0.0f,       0.0f,
              0.0f,   0.4f,   0.0f,       0.0f,
              0.0f,   0.0f,   -0.25f,     0.0f,
              0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);

    Vector2 projectionSizeRectangle(5.0f, 4.0f);
    camera.setOrthographic(projectionSizeRectangle, 1, 9);

    Matrix4 rectangle(0.4f,   0.0f,   0.0f,       0.0f,
                      0.0f,   0.5f,   0.0f,       0.0f,
                      0.0f,   0.0f,   -0.25f,     0.0f,
                      0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), rectangle);
    CORRADE_COMPARE(camera.projectionSize(), projectionSizeRectangle);
}

void CameraTest::perspective() {
    Camera3D camera;
    camera.setPerspective(deg(27.0f), 32.0f, 100);

    Matrix4 a(4.1652994f, 0.0f,       0.0f,        0.0f,
              0.0f,       4.1652994f, 0.0f,        0.0f,
              0.0f,       0.0f,      -1.9411764f, -1.0f,
              0.0f,       0.0f,      -94.1176452f, 0.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(0.48015756f));
}

void CameraTest::projectionSizeViewport() {
    Camera3D camera;
    camera.setViewport({200, 300});
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 2.0f));

    camera.setAspectRatioPolicy(Camera3D::AspectRatioPolicy::Extend);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 3.0f));

    camera.setAspectRatioPolicy(Camera3D::AspectRatioPolicy::Clip);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(4.0f/3.0f, 2.0f));
}

}}}
