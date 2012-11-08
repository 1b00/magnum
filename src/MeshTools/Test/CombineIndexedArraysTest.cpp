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

#include "CombineIndexedArraysTest.h"

#include <sstream>

#include "MeshTools/CombineIndexedArrays.h"

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CombineIndexedArraysTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

CombineIndexedArraysTest::CombineIndexedArraysTest() {
    addTests(&CombineIndexedArraysTest::wrongIndexCount,
             &CombineIndexedArraysTest::combine);
}

void CombineIndexedArraysTest::wrongIndexCount() {
    stringstream ss;
    Error::setOutput(&ss);
    vector<uint32_t> array;
    vector<uint32_t> result = MeshTools::combineIndexedArrays(
        tuple<const vector<uint32_t>&, vector<uint32_t>&>(vector<uint32_t>{0, 1, 0}, array),
        tuple<const vector<uint32_t>&, vector<uint32_t>&>(vector<uint32_t>{3, 4}, array));

    CORRADE_COMPARE(result.size(), 0);
    CORRADE_COMPARE(ss.str(), "MeshTools::combineIndexedArrays(): index arrays don't have the same length, nothing done.\n");
}

void CombineIndexedArraysTest::combine() {
    vector<uint32_t> array1{ 0, 1 };
    vector<uint32_t> array2{ 0, 1, 2, 3, 4 };
    vector<uint32_t> array3{ 0, 1, 2, 3, 4, 5, 6, 7 };

    vector<uint32_t> result = MeshTools::combineIndexedArrays(
        tuple<const vector<uint32_t>&, vector<uint32_t>&>(vector<uint32_t>{0, 1, 0}, array1),
        tuple<const vector<uint32_t>&, vector<uint32_t>&>(vector<uint32_t>{3, 4, 3}, array2),
        tuple<const vector<uint32_t>&, vector<uint32_t>&>(vector<uint32_t>{6, 7, 6}, array3));

    CORRADE_COMPARE(result, (vector<uint32_t>{0, 1, 0}));
    CORRADE_COMPARE(array1, (vector<uint32_t>{0, 1}));
    CORRADE_COMPARE(array2, (vector<uint32_t>{3, 4}));
    CORRADE_COMPARE(array3, (vector<uint32_t>{6, 7}));
}

}}}
