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

#include "Framebuffer.h"

#include "BufferImage.h"
#include "Context.h"
#include "Extensions.h"
#include "Image.h"
#include "Renderbuffer.h"
#include "Texture.h"

#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

Framebuffer::RenderbufferImplementation Framebuffer::renderbufferImplementation = &Framebuffer::renderbufferImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
Framebuffer::Texture1DImplementation Framebuffer::texture1DImplementation = &Framebuffer::texture1DImplementationDefault;
#endif
Framebuffer::Texture2DImplementation Framebuffer::texture2DImplementation = &Framebuffer::texture2DImplementationDefault;
Framebuffer::Texture3DImplementation Framebuffer::texture3DImplementation = &Framebuffer::texture3DImplementationDefault;

const Framebuffer::DrawAttachment Framebuffer::DrawAttachment::None = Framebuffer::DrawAttachment(GL_NONE);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Depth = Framebuffer::BufferAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Stencil = Framebuffer::BufferAttachment(GL_STENCIL_ATTACHMENT);
#ifndef MAGNUM_TARGET_GLES2
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::DepthStencil = Framebuffer::BufferAttachment(GL_DEPTH_STENCIL_ATTACHMENT);
#endif
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Depth = Framebuffer::InvalidationAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Stencil = Framebuffer::InvalidationAttachment(GL_STENCIL_ATTACHMENT);

Framebuffer::Framebuffer(const Rectanglei& viewport) {
    _viewport = viewport;

    glGenFramebuffers(1, &_id);
}

Framebuffer::~Framebuffer() {
    /* If bound, remove itself from state */
    Implementation::FramebufferState* state = Context::current()->state().framebuffer;
    if(state->readBinding == _id) state->readBinding = 0;
    if(state->drawBinding == _id) state->drawBinding = 0;

    glDeleteFramebuffers(1, &_id);
}

Framebuffer& Framebuffer::mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        if(it->first > max) max = it->first;

    /* Create linear array from associative */
    /** @todo C++14: use VLA to avoid heap allocation */
    GLenum* _attachments = new GLenum[max+1];
    std::fill_n(_attachments, max, GL_NONE);
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it->first] = GLenum(it->second);

    (this->*drawBuffersImplementation)(max+1, _attachments);
    delete[] _attachments;
    return *this;
}

void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments) {
    /** @todo C++14: use VLA to avoid heap allocation */
    GLenum* _attachments = new GLenum[attachments.size()];
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    invalidateImplementation(attachments.size(), _attachments);

    delete[] _attachments;
}

void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments, const Rectanglei& rectangle) {
    /** @todo C++14: use VLA to avoid heap allocation */
    GLenum* _attachments = new GLenum[attachments.size()];
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    invalidateImplementation(attachments.size(), _attachments, rectangle);

    delete[] _attachments;
}

Framebuffer& Framebuffer::attachTexture2D(BufferAttachment attachment, Texture2D& texture, Int mipLevel) {
    /** @todo Check for texture target compatibility */
    (this->*texture2DImplementation)(attachment, GLenum(texture.target()), texture.id(), mipLevel);
    return *this;
}

void Framebuffer::initializeContextBasedFunctionality(Context& context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Framebuffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDSA;
        texture1DImplementation = &Framebuffer::texture1DImplementationDSA;
        texture2DImplementation = &Framebuffer::texture2DImplementationDSA;
        texture3DImplementation = &Framebuffer::texture3DImplementationDSA;
    }
    #else
    static_cast<void>(context);
    #endif
}

void Framebuffer::renderbufferImplementationDefault(BufferAttachment attachment, Renderbuffer& renderbuffer) {
    glFramebufferRenderbuffer(GLenum(bindInternal()), GLenum(attachment), GL_RENDERBUFFER, renderbuffer.id());
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::renderbufferImplementationDSA(BufferAttachment attachment, Renderbuffer& renderbuffer) {
    glNamedFramebufferRenderbufferEXT(_id, GLenum(attachment), GL_RENDERBUFFER, renderbuffer.id());
}

void Framebuffer::texture1DImplementationDefault(BufferAttachment attachment, Texture1D& texture, GLint mipLevel) {
    glFramebufferTexture1D(GLenum(bindInternal()), GLenum(attachment), static_cast<GLenum>(texture.target()), texture.id(), mipLevel);
}

void Framebuffer::texture1DImplementationDSA(BufferAttachment attachment, Texture1D& texture, GLint mipLevel) {
    glNamedFramebufferTexture1DEXT(_id, GLenum(attachment), GLenum(texture.target()), texture.id(), mipLevel);
}
#endif

void Framebuffer::texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture2D(GLenum(bindInternal()), GLenum(attachment), textureTarget, textureId, mipLevel);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::texture2DImplementationDSA(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel) {
    glNamedFramebufferTexture2DEXT(_id, GLenum(attachment), textureTarget, textureId, mipLevel);
}
#endif

void Framebuffer::texture3DImplementationDefault(BufferAttachment attachment, Texture3D& texture, GLint mipLevel, GLint layer) {
    /** @todo Check for texture target compatibility */
    /** @todo Get some extension wrangler for glFramebufferTexture3D() (extension only) */
    #ifndef MAGNUM_TARGET_GLES
    glFramebufferTexture3D(GLenum(bindInternal()), GLenum(attachment), static_cast<GLenum>(texture.target()), texture.id(), mipLevel, layer);
    #else
    static_cast<void>(attachment);
    static_cast<void>(texture);
    static_cast<void>(mipLevel);
    static_cast<void>(layer);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::texture3DImplementationDSA(BufferAttachment attachment, Texture3D& texture, GLint mipLevel, GLint layer) {
    glNamedFramebufferTexture3DEXT(_id, GLenum(attachment), GLenum(texture.target()), texture.id(), mipLevel, layer);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const Framebuffer::Status value) {
    switch(value) {
        #define _c(value) case Framebuffer::Status::value: return debug << "Framebuffer::Status::" #value;
        _c(Complete)
        _c(IncompleteAttachment)
        _c(IncompleteMissingAttachment)
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteDrawBuffer)
        _c(IncompleteReadBuffer)
        #endif
        _c(Unsupported)
        _c(IncompleteMultisample)
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteLayerTargets)
        #endif
        #undef _c
    }

    return debug << "Framebuffer::Status::(invalid)";
}
#endif

}
