#ifndef Magnum_Renderbuffer_h
#define Magnum_Renderbuffer_h
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

/** @file
 * @brief Class @ref Magnum::Renderbuffer
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Magnum.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#include "RenderbufferFormat.h"
#endif

namespace Magnum {

namespace Implementation { struct FramebufferState; }

/**
@brief %Renderbuffer

Attachable to framebuffer as render target, see @ref Framebuffer documentation
for more information.

## Performance optimizations

The engine tracks currently bound renderbuffer to avoid unnecessary calls to
@fn_gl{BindRenderbuffer} in @ref setStorage(). %Renderbuffer limits and
implementation-defined values (such as @ref maxSize()) are cached, so repeated
queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} is available, function
@ref setStorage() uses DSA to avoid unnecessary calls to @fn_gl{BindRenderbuffer}.
See its documentation for more information.

@requires_gl30 %Extension @extension{ARB,framebuffer_object}
*/
class MAGNUM_EXPORT Renderbuffer: public AbstractObject {
    friend struct Implementation::FramebufferState;

    public:
        /**
         * @brief Max supported renderbuffer size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setStorage(), @ref setStorageMultisample(), @fn_gl{Get}
         *      with @def_gl{MAX_RENDERBUFFER_SIZE}
         */
        static Int maxSize();

        /**
         * @brief Max supported sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither OpenGL ES 3.0 nor ES extension
         * @es_extension{ANGLE,framebuffer_multisample} /
         * @es_extension{NV,framebuffer_multisample} is available, returns `0`.
         * @see @ref setStorageMultisample(), @fn_gl{Get} with @def_gl{MAX_SAMPLES}
         */
        static Int maxSamples();

        /**
         * @brief Constructor
         *
         * Generates new OpenGL renderbuffer object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not supported, the renderbuffer is created
         * on first use.
         * @see @fn_gl{CreateRenderbuffers}, eventually @fn_gl{GenRenderbuffers}
         */
        explicit Renderbuffer();

        /** @brief Copying is not allowed */
        Renderbuffer(const Renderbuffer&) = delete;

        /** @brief Move constructor */
        Renderbuffer(Renderbuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL renderbuffer object.
         * @see @fn_gl{DeleteRenderbuffers}
         */
        ~Renderbuffer();

        /** @brief Copying is not allowed */
        Renderbuffer& operator=(const Renderbuffer&) = delete;

        /** @brief Move assignment */
        Renderbuffer& operator=(Renderbuffer&& other) noexcept;

        /** @brief OpenGL internal renderbuffer ID */
        GLuint id() const { return _id; }

        /**
         * @brief %Renderbuffer label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{RENDERBUFFER}
         */
        std::string label();

        /**
         * @brief Set renderbuffer label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} or
         *      @fn_gl_extension2{LabelObject,EXT,debug_label} with
         *      @def_gl{RENDERBUFFER}
         */
        Renderbuffer& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Renderbuffer& setLabel(const char(&label)[size]) {
            return setLabelInternal(label);
        }

        /**
         * @brief Set renderbuffer storage
         * @param internalFormat    Internal format
         * @param size              %Renderbuffer size
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref maxSize(), @fn_gl{BindRenderbuffer}, @fn_gl{RenderbufferStorage}
         *      or @fn_gl_extension{NamedRenderbufferStorage,EXT,direct_state_access}
         */
        void setStorage(RenderbufferFormat internalFormat, const Vector2i& size);

        /**
         * @brief Set multisample renderbuffer storage
         * @param samples           Sample count
         * @param internalFormat    Internal format
         * @param size              %Renderbuffer size
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref maxSize(), @ref maxSamples(), @fn_gl{BindRenderbuffer},
         *      @fn_gl{RenderbufferStorageMultisample} or @fn_gl_extension{NamedRenderbufferStorageMultisample,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_multisample}
         *      or @es_extension{NV,framebuffer_multisample} in OpenGL ES 2.0
         * @todo How about @es_extension{APPLE,framebuffer_multisample}?
         * @todo NaCl has @fn_gl_extension{RenderbufferStorageMultisample,EXT,multisampled_render_to_texture}
         */
        void setStorageMultisample(Int samples, RenderbufferFormat internalFormat, const Vector2i& size);

    private:
        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationDSA();
        #endif

        void MAGNUM_LOCAL createIfNotAlready();

        Renderbuffer& setLabelInternal(Containers::ArrayReference<const char> label);

        void MAGNUM_LOCAL storageImplementationDefault(RenderbufferFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSAEXT(RenderbufferFormat internalFormat, const Vector2i& size);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL storageMultisampleImplementationDefault(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageMultisampleImplementationDSAEXT(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #endif
        #else
        void MAGNUM_LOCAL storageMultisampleImplementationANGLE(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        void MAGNUM_LOCAL storageMultisampleImplementationNV(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #endif

        void MAGNUM_LOCAL bind();

        GLuint _id;
        bool _created; /* see createIfNotAlready() for details */
};

inline Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept: _id{other._id}, _created{other._created} {
    other._id = 0;
}

inline Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept {
    std::swap(_id, other._id);
    std::swap(_created, other._created);
    return *this;
}

}

#endif
