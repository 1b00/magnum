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

#include "AbstractFramebuffer.h"

#include "BufferImage.h"
#include "Context.h"
#include "Extensions.h"
#include "Image.h"

#include "Implementation/FramebufferState.h"
#include "Implementation/State.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
AbstractFramebuffer::DrawBuffersImplementation AbstractFramebuffer::drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDefault;
AbstractFramebuffer::DrawBufferImplementation AbstractFramebuffer::drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDefault;
AbstractFramebuffer::ReadBufferImplementation AbstractFramebuffer::readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDefault;

AbstractFramebuffer::Target AbstractFramebuffer::readTarget = AbstractFramebuffer::Target::ReadDraw;
AbstractFramebuffer::Target AbstractFramebuffer::drawTarget = AbstractFramebuffer::Target::ReadDraw;
#endif

void AbstractFramebuffer::bind(Target target) {
    bindInternal(target);
    setViewportInternal();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractFramebuffer::bindInternal(Target target) {
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;

    /* If already bound, done, otherwise update tracked state */
    if(target == Target::Read) {
        if(state->readBinding == _id) return;
        state->readBinding = _id;
    } else if(target == Target::Draw) {
        if(state->drawBinding == _id) return;
        state->drawBinding = _id;
    } else if(target == Target::ReadDraw) {
        if(state->readBinding == _id && state->drawBinding == _id) return;
        state->readBinding = state->drawBinding = _id;
    } else CORRADE_INTERNAL_ASSERT(false);

    glBindFramebuffer(static_cast<GLenum>(target), _id);
}

AbstractFramebuffer::Target AbstractFramebuffer::bindInternal() {
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;

    /* Return target to which the framebuffer is already bound */
    if(state->readBinding == _id && state->drawBinding == _id)
        return Target::ReadDraw;
    if(state->readBinding == _id)
        return Target::Read;
    if(state->drawBinding == _id)
        return Target::Draw;

    /* Or bind it, if not already */
    state->readBinding = _id;
    if(readTarget == Target::ReadDraw) state->drawBinding = _id;

    glBindFramebuffer(GLenum(readTarget), _id);
    return readTarget;
}
#endif

void AbstractFramebuffer::blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Rectanglei& sourceRectangle, const Rectanglei& destinationRectangle, AbstractFramebuffer::BlitMask mask, AbstractFramebuffer::BlitFilter filter) {
    source.bindInternal(AbstractFramebuffer::Target::Read);
    destination.bindInternal(AbstractFramebuffer::Target::Draw);
    /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glBlitFramebuffer(sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), static_cast<GLbitfield>(mask), static_cast<GLenum>(filter));
    #else
    static_cast<void>(sourceRectangle);
    static_cast<void>(destinationRectangle);
    static_cast<void>(mask);
    static_cast<void>(filter);
    #endif
}

AbstractFramebuffer* AbstractFramebuffer::setViewport(const Rectanglei& rectangle) {
    _viewport = rectangle;

    /* Update the viewport if the framebuffer is currently bound */
    if(Context::current()->state()->framebuffer->drawBinding == _id)
        setViewportInternal();

    return this;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractFramebuffer::setViewportInternal() {
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;

    CORRADE_INTERNAL_ASSERT(state->drawBinding == _id);

    /* Already up-to-date, nothing to do */
    if(state->viewport == _viewport)
        return;

    /* Update the state and viewport */
    state->viewport = _viewport;
    glViewport(_viewport.left(), _viewport.bottom(), _viewport.width(), _viewport.height());
}
#endif

void AbstractFramebuffer::clear(ClearMask mask) {
    bindInternal(drawTarget);
    glClear(static_cast<GLbitfield>(mask));
}

void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image) {
    bindInternal(readTarget);
    char* data = new char[AbstractImage::pixelSize(format, type)*size.product()];
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    image->setData(size, format, type, data);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage) {
    bindInternal(readTarget);
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image->size() != size || image->format() != format || image->type() != type)
        image->setData(size, format, type, nullptr, usage);

    image->buffer()->bind(Buffer::Target::PixelPack);
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), nullptr);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractFramebuffer::invalidateImplementation(GLsizei count, GLenum* attachments) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glInvalidateFramebuffer(GLenum(bindInternal()), count, attachments);
    #else
    //glDiscardFramebufferEXT(GLenum(bindInternal()), count, attachments);
    static_cast<void>(count);
    static_cast<void>(attachments);
    #endif
}

void AbstractFramebuffer::invalidateImplementation(GLsizei count, GLenum* attachments, const Rectanglei& rectangle) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glInvalidateSubFramebuffer(GLenum(bindInternal()), count, attachments, rectangle.left(), rectangle.bottom(), rectangle.width(), rectangle.height());
    #else
    //glDiscardSubFramebufferEXT(GLenum(bindInternal()), count, attachments, rectangle.left(), rectangle.bottom(), rectangle.width(), rectangle.height());
    static_cast<void>(count);
    static_cast<void>(attachments);
    static_cast<void>(rectangle);
    #endif
}
#endif

void AbstractFramebuffer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::framebuffer_blit>()) {
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::EXT::framebuffer_blit::string() << "features";

        readTarget = Target::Read;
        drawTarget = Target::Draw;
    }

    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSA;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSA;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSA;
    }
    #else
    static_cast<void>(context);
    #endif
}

void AbstractFramebuffer::drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(drawTarget);
    glDrawBuffers(count, buffers);
    #else
    static_cast<void>(count);
    static_cast<void>(buffers);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers) {
    glFramebufferDrawBuffersEXT(_id, count, buffers);
}
#endif

void AbstractFramebuffer::drawBufferImplementationDefault(GLenum buffer) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(drawTarget);
    glDrawBuffer(buffer);
    #else
    static_cast<void>(buffer);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBufferImplementationDSA(GLenum buffer) {
    glFramebufferDrawBufferEXT(_id, buffer);
}
#endif

void AbstractFramebuffer::readBufferImplementationDefault(GLenum buffer) {
    /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(readTarget);
    glReadBuffer(buffer);
    #else
    static_cast<void>(buffer);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::readBufferImplementationDSA(GLenum buffer) {
    glFramebufferReadBufferEXT(_id, buffer);
}
#endif

}
