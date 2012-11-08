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

#include "ResourceManagerTest.h"

#include <sstream>

#include "ResourceManager.h"

using namespace std;
using namespace Corrade::Utility;

CORRADE_TEST_MAIN(Magnum::Test::ResourceManagerTest)

namespace Magnum { namespace Test {

size_t Data::count = 0;

ResourceManagerTest::ResourceManagerTest() {
    rm = new ResourceManager;

    addTests(&ResourceManagerTest::state,
             &ResourceManagerTest::referenceCountedPolicy,
             &ResourceManagerTest::manualPolicy,
             &ResourceManagerTest::destroy);
}

void ResourceManagerTest::state() {
    ResourceKey questionKey("the-question");
    rm->set(questionKey, new int32_t(10), ResourceDataState::Mutable, ResourcePolicy::Resident);
    Resource<int32_t> theQuestion = rm->get<int32_t>(questionKey);
    CORRADE_VERIFY(theQuestion.state() == ResourceState::Mutable);
    CORRADE_COMPARE(*theQuestion, 10);

    /* Check that hash function is working properly */
    ResourceKey answerKey("the-answer");
    rm->set(answerKey, new int32_t(42), ResourceDataState::Final, ResourcePolicy::Resident);
    Resource<int32_t> theAnswer = rm->get<int32_t>(answerKey);
    CORRADE_VERIFY(theAnswer.state() == ResourceState::Final);
    CORRADE_COMPARE(*theAnswer, 42);

    CORRADE_COMPARE(rm->count<int32_t>(), 2);

    /* Cannot change already final resource */
    stringstream out;
    Error::setOutput(&out);
    rm->set(answerKey, new int32_t(43), ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(out.str(), "ResourceManager: cannot change already final resource\n");

    /* Check non-final resource changes */
    rm->set(questionKey, new int32_t(20), ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_VERIFY(theQuestion.state() == ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 20);
}

void ResourceManagerTest::referenceCountedPolicy() {
    ResourceKey dataRefCountKey("dataRefCount");

    /* Reference counted resources must be requested first */
    {
        rm->set(dataRefCountKey, new Data(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
        CORRADE_COMPARE(rm->count<Data>(), 0);
        Resource<Data> data = rm->get<Data>(dataRefCountKey);
        CORRADE_VERIFY(data.state() == ResourceState::NotLoaded);
        CORRADE_COMPARE(Data::count, 0);
    } {
        Resource<Data> data = rm->get<Data>(dataRefCountKey);
        CORRADE_COMPARE(rm->count<Data>(), 1);
        CORRADE_VERIFY(data.state() == ResourceState::NotLoaded);
        rm->set(dataRefCountKey, new Data(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
        CORRADE_VERIFY(data.state() == ResourceState::Final);
        CORRADE_COMPARE(Data::count, 1);
    }

    CORRADE_COMPARE(rm->count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::manualPolicy() {
    ResourceKey dataKey("data");

    /* Manual free */
    {
        rm->set(dataKey, new Data(), ResourceDataState::Mutable, ResourcePolicy::Manual);
        Resource<Data> data = rm->get<Data>(dataKey);
        rm->free();
    }

    CORRADE_COMPARE(rm->count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
    rm->free();
    CORRADE_COMPARE(rm->count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);

    rm->set(dataKey, new Data(), ResourceDataState::Mutable, ResourcePolicy::Manual);
    CORRADE_COMPARE(rm->count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
}

void ResourceManagerTest::destroy() {
    delete rm;
    rm = nullptr;
    CORRADE_COMPARE(Data::count, 0);
}

}}
