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

#include "SubdivideTest.h"

#include <sstream>

#include "MeshTools/Clean.h"
#include "MeshTools/Subdivide.h"

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::SubdivideTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

SubdivideTest::SubdivideTest() {
    addTests(&SubdivideTest::wrongIndexCount,
             &SubdivideTest::subdivide);
}

void SubdivideTest::wrongIndexCount() {
    stringstream ss;
    Error::setOutput(&ss);

    vector<Vector1> positions;
    vector<uint32_t> indices{0, 1};
    MeshTools::subdivide(indices, positions, interpolator);
    CORRADE_COMPARE(ss.str(), "MeshTools::subdivide(): index count is not divisible by 3!\n");
}

void SubdivideTest::subdivide() {
    vector<Vector1> positions{0, 2, 6, 8};
    vector<uint32_t> indices{0, 1, 2, 1, 2, 3};
    MeshTools::subdivide(indices, positions, interpolator);

    CORRADE_COMPARE(indices.size(), 24);

    CORRADE_VERIFY(positions == (vector<Vector1>{0, 2, 6, 8, 1, 4, 3, 4, 7, 5}));
    CORRADE_COMPARE(indices, (vector<uint32_t>{4, 5, 6, 7, 8, 9, 0, 4, 6, 4, 1, 5, 6, 5, 2, 1, 7, 9, 7, 2, 8, 9, 8, 3}));

    MeshTools::clean(indices, positions);

    /* Positions 0, 1, 2, 3, 4, 5, 6, 7, 8 */
    CORRADE_COMPARE(positions.size(), 9);
}

}}}
