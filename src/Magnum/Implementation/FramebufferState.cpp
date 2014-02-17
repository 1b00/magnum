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

#include "FramebufferState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Renderbuffer.h"

namespace Magnum { namespace Implementation {

FramebufferState::FramebufferState(Context& context, std::vector<std::string>& extensions): readBinding(0), drawBinding(0), renderbufferBinding(0), maxDrawBuffers(0), maxColorAttachments(0), maxRenderbufferSize(0), maxSamples(0)
    #ifndef MAGNUM_TARGET_GLES
    , maxDualSourceDrawBuffers(0)
    #endif
{
    /* DSA/non-DSA implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDSA;
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSA;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSA;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSA;

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDSA;
        texture1DImplementation = &Framebuffer::texture1DImplementationDSA;
        texture2DImplementation = &Framebuffer::texture2DImplementationDSA;
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDSA;

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDSA;
    } else
    #endif
    {
        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDefault;
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDefault;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDefault;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDefault;

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        texture1DImplementation = &Framebuffer::texture1DImplementationDefault;
        #endif
        texture2DImplementation = &Framebuffer::texture2DImplementationDefault;
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDefault;

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDefault;
    }

    /* Framebuffer binding on ES2 */
    #ifdef MAGNUM_TARGET_GLES2
    /* Optimistically set separate binding targets and check if one of the
       extensions providing them is available */
    readTarget = FramebufferTarget::Read;
    drawTarget = FramebufferTarget::Draw;

    if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::ANGLE::framebuffer_blit::string());

    } else if(context.isExtensionSupported<Extensions::GL::APPLE::framebuffer_multisample>()) {
        extensions.push_back(Extensions::GL::APPLE::framebuffer_multisample::string());

    } else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::NV::framebuffer_blit::string());

    /* NV_framebuffer_multisample requires NV_framebuffer_blit, which has these
       enums. However, on my system only NV_framebuffer_multisample is
       supported, but NV_framebuffer_blit isn't. I will hold my breath and
       assume these enums are available. */
    } else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>()) {
        extensions.push_back(Extensions::GL::NV::framebuffer_multisample::string());

    /* If no such extension is available, reset back to unified target */
    } else readTarget = drawTarget = FramebufferTarget::ReadDraw;
    #endif

    /* Framebuffer reading implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::robustness>())
    #else
    if(context.isExtensionSupported<Extensions::GL::EXT::robustness>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.push_back(Extensions::GL::ARB::robustness::string());
        #else
        extensions.push_back(Extensions::GL::EXT::robustness::string());
        #endif

        readImplementation = &AbstractFramebuffer::readImplementationRobustness;
    } else readImplementation = &AbstractFramebuffer::readImplementationDefault;

    /* Multisample renderbuffer storage implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        /* Extension added above */

        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDSA;
    } else
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>()) {
            extensions.push_back(Extensions::GL::ANGLE::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationANGLE;
        } else if (context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>()) {
            extensions.push_back(Extensions::GL::NV::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationNV;
        } else renderbufferStorageMultisampleImplementation = nullptr;
        #else
        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDefault;
        #endif
    }
}

}}
