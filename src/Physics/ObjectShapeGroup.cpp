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

#include "ObjectShapeGroup.h"

#include "Physics/AbstractShape.h"
#include "Physics/ObjectShape.h"

namespace Magnum { namespace Physics {

template<UnsignedInt dimensions> void ObjectShapeGroup<dimensions>::setClean() {
    /* Clean all objects */
    if(!this->isEmpty()) {
        std::vector<SceneGraph::AbstractObject<dimensions>*> objects(this->size());
        for(std::size_t i = 0; i != this->size(); ++i)
            objects[i] = (*this)[i]->object();

        objects[0]->setClean(objects);
    }

    dirty = false;
}

template<UnsignedInt dimensions> ObjectShape<dimensions>* ObjectShapeGroup<dimensions>::firstCollision(const ObjectShape<dimensions>* shape) {
    /* Nothing to test with, done */
    if(!shape->shape()) return nullptr;

    setClean();
    for(std::size_t i = 0; i != this->size(); ++i)
        if((*this)[i] != shape && (*this)[i]->shape() && (*this)[i]->shape()->collides(shape->shape()))
            return (*this)[i];

    return nullptr;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_PHYSICS_EXPORT ObjectShapeGroup<2>;
template class MAGNUM_PHYSICS_EXPORT ObjectShapeGroup<3>;
#endif

}}
