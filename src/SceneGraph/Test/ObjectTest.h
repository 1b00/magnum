#ifndef Magnum_SceneGraph_Test_ObjectTest_h
#define Magnum_SceneGraph_Test_ObjectTest_h
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

#include <TestSuite/Tester.h>

namespace Magnum { namespace SceneGraph { namespace Test {

class ObjectTest: public Corrade::TestSuite::Tester<ObjectTest> {
    public:
        ObjectTest();

        void parenting();
        void scene();
        void absoluteTransformation();
        void transformations();
        void setClean();
        void bulkSetClean();
};

}}}

#endif
