/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "MeshState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

namespace Magnum { namespace Implementation {

MeshState::MeshState(Context& context, std::vector<std::string>& extensions): currentVAO(0)
    #ifndef MAGNUM_TARGET_GLES2
    , maxElementsIndices(0), maxElementsVertices(0)
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(context.isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #else
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.push_back(Extensions::GL::APPLE::vertex_array_object::string());
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions.push_back(Extensions::GL::OES::vertex_array_object::string());
        #endif

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

            attributePointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationDSA;
        } else
        #endif
        {
            attributePointerImplementation = &Mesh::attributePointerImplementationVAO;
            #ifndef MAGNUM_TARGET_GLES2
            attributeIPointerImplementation = &Mesh::attributePointerImplementationVAO;
            #ifndef MAGNUM_TARGET_GLES
            attributeLPointerImplementation = &Mesh::attributePointerImplementationVAO;
            #endif
            #endif
        }

        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationVAO;
        bindImplementation = &Mesh::bindImplementationVAO;
        unbindImplementation = &Mesh::unbindImplementationVAO;
    }
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    else {
        createImplementation = &Mesh::createImplementationDefault;
        destroyImplementation = &Mesh::destroyImplementationDefault;
        attributePointerImplementation = &Mesh::attributePointerImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        attributeIPointerImplementation = &Mesh::attributePointerImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        attributeLPointerImplementation = &Mesh::attributePointerImplementationDefault;
        #endif
        #endif
        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationDefault;
        bindImplementation = &Mesh::bindImplementationDefault;
        unbindImplementation = &Mesh::unbindImplementationDefault;
    }
    #endif
}

}}
