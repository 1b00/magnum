#ifndef Magnum_Framebuffer_h
#define Magnum_Framebuffer_h
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

/** @file
 * @brief Class Magnum::Framebuffer
 */

#include <Utility/Set.h>

#include "BufferedImage.h"
#include "CubeMapTexture.h"
#include "Color.h"
#include "Image.h"
#include "Renderbuffer.h"

namespace Magnum {

/** @ingroup textures
@brief %Framebuffer

Provides operations with framebuffers (configuring, clearing, blitting...) and
creation and attaching of named framebuffers.
*/
class MAGNUM_EXPORT Framebuffer {
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer& operator=(Framebuffer&& other) = delete;

    public:
        /**
         * @brief Features
         *
         * If not specified otherwise, all features are disabled by default.
         * @see setFeature()
         */
        enum class Feature: GLenum {
            AlphaBlending = GL_BLEND,       /**< Alpha blending */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Depth clamping. If enabled, ignores near and far clipping plane.
             * @requires_gl
             * @requires_gl32 Extension @extension{ARB,depth_clamp}
             */
            DepthClamp = GL_DEPTH_CLAMP,
            #endif

            DepthTest = GL_DEPTH_TEST,      /**< Depth test */
            StencilTest = GL_STENCIL_TEST,  /**< Stencil test */
            Dithering = GL_DITHER,          /**< Dithering (enabled by default) */
            FaceCulling = GL_CULL_FACE      /**< Back face culling */
        };

        /**
         * @brief Mask for clearing
         *
         * @see ClearMask, clear(), clear(ClearMask)
         */
        enum class Clear: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        typedef Corrade::Utility::Set<Clear, GLbitfield> ClearMask; /**< @brief Mask for clearing */

        /**
         * @brief %Framebuffer target
         *
         * @see bind(), bindDefault()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * For reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             */
            Read = GL_READ_FRAMEBUFFER,

            /**
             * For drawing only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             */
            Draw = GL_DRAW_FRAMEBUFFER,
            #endif

            ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
        };

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw attachment for default framebuffer
         *
         * @see mapDefaultForDraw()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DefaultDrawAttachment: GLenum {
            None = GL_NONE,                 /**< Don't use the output. */
            BackLeft = GL_BACK_LEFT,        /**< Write output to back left framebuffer. */
            BackRight = GL_BACK_RIGHT,      /**< Write output to back right framebuffer. */
            FrontLeft = GL_FRONT_LEFT,      /**< Write output to front left framebuffer. */
            FrontRight = GL_FRONT_RIGHT     /**< Write output to front right framebuffer. */
        };

        /**
         * @brief Read attachment for default framebuffer
         *
         * @see mapDefaultForRead()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DefaultReadAttachment: GLenum {
            FrontLeft = GL_FRONT_LEFT,      /**< Read from front left framebuffer. */
            FrontRight = GL_FRONT_RIGHT,    /**< Read from front right framebuffer. */
            BackLeft = GL_BACK_LEFT,        /**< Read from back left framebuffer. */
            BackRight = GL_BACK_RIGHT,      /**< Read from back right framebuffer. */
            Left = GL_LEFT,                 /**< Read from left framebuffers. */
            Right = GL_RIGHT,               /**< Read from right framebuffers. */
            Front = GL_FRONT,               /**< Read from front framebuffers. */
            Back = GL_BACK,                 /**< Read from back framebuffers. */
            FrontAndBack = GL_FRONT_AND_BACK /**< Read from front and back framebuffers. */
        };
        #endif

        /**
         * @brief Attachment for depth/stencil part of fragment shader output
         *
         * @see attachRenderbuffer(Target, DepthStencilAttachment, Renderbuffer*),
         *      attachTexture1D(Target, DepthStencilAttachment, Texture1D*, GLint),
         *      attachTexture2D(Target, DepthStencilAttachment, Texture2D*, GLint),
         *      attachCubeMapTexture(Target, DepthStencilAttachment, CubeMapTexture*, CubeMapTexture::Coordinate, GLint),
         *      attachTexture3D(Target, DepthStencilAttachment, Texture3D*, GLint)
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DepthStencilAttachment: GLenum {
            Depth = GL_DEPTH_ATTACHMENT,    /**< Depth output only. */

            Stencil = GL_STENCIL_ATTACHMENT /**< Stencil output only. */

            #ifndef MAGNUM_TARGET_GLES
            ,
            /**
             * Both depth and stencil output.
             */
            DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
            #endif
        };

        /**
         * @brief Output mask for blitting
         *
         * Specifies which data are copied when performing blit operation
         * using blit().
         * @see BlitMask
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class Blit: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        /**
         * @brief Output mask for blitting
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        typedef Corrade::Utility::Set<Blit, GLbitfield> BlitMask;

        /** @brief Set feature */
        static void setFeature(Feature feature, bool enabled);

        /**
         * @brief Set clear color
         *
         * Initial value is `{0.0f, 0.0f, 0.0f, 1.0f}`.
         */
        inline static void setClearColor(const Color4<GLfloat>& color) {
            glClearColor(color.r(), color.g(), color.b(), color.a());
        }

        /**
         * @brief Clear framebuffer
         *
         * Clears color buffer, depth and stencil buffer in currently active
         * framebuffer. If depth or stencil test is not enabled, it doesn't
         * clear these buffers.
         *
         * @see setFeature(), clear(ClearMask)
         */
        inline static void clear() { glClear(static_cast<GLbitfield>(clearMask)); }

        /**
         * @brief Clear specified buffers in framebuffer
         *
         * @see clear()
         */
        inline static void clear(ClearMask mask) { glClear(static_cast<GLbitfield>(mask)); }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set clear depth
         *
         * Initial value is `1.0`.
         * @requires_gl See setClearDepth(GLfloat), which is supported in OpenGL ES.
         */
        inline static void setClearDepth(GLdouble depth) { glClearDepth(depth); }
        #endif

        /**
         * @overload
         *
         * @requires_gl41 Extension @extension{ARB,ES2_compatibility}
         */
        inline static void setClearDepth(GLfloat depth) { glClearDepthf(depth); }

        /**
         * @brief Set clear stencil
         *
         * Initial value is `0`.
         */
        inline static void setClearStencil(GLint stencil) { glClearStencil(stencil); }

        /**
         * @brief Set viewport size
         *
         * Call when window size changes.
         * @see Camera::setViewport()
         */
        static void setViewport(const Math::Vector2<GLint>& position, const Math::Vector2<GLsizei>& size);

        /**
         * @brief Bind default framebuffer to given target
         * @param target     %Target
         *
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline static void bindDefault(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), 0);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Map given attachments of default framebuffer for drawing
         * @param attachments       Default attachments. If any value is
         *      DefaultAttachment::None, given output is not used.
         *
         * If used for mapping output of fragment shader, the order must be as
         * specified by the shader (see AbstractShaderProgram documentation).
         * If used for blit(), the order is not important. Each used attachment
         * should have either renderbuffer or texture attached for writing to
         * work properly.
         * @see mapForDraw(), mapDefaultForRead()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments);

        /**
         * @brief Map given attachment of default framebuffer for reading
         * @param attachment        Default attachment
         *
         * Each used attachment should have either renderbuffer or texture
         * attached to work properly.
         * @see mapForRead(), mapDefaultForDraw()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline static void mapDefaultForRead(DefaultReadAttachment attachment) {
            bindDefault(Target::Read);
            glReadBuffer(static_cast<GLenum>(attachment));
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from read to draw framebuffer
         * @param bottomLeft        Bottom left coordinates of source rectangle
         * @param topRight          Top right coordinates of source rectangle
         * @param destinationBottomLeft Bottom left coordinates of destination rectangle
         * @param destinationTopRight Top right coordinates of destination
         *      rectangle
         * @param blitMask          Blit mask
         * @param filter            Interpolation applied if the image is
         *      stretched
         *
         * See mapForRead() / mapDefaultForRead() and mapForDraw() /
         * mapDefaultForDraw() for binding particular framebuffer for reading
         * and drawing. If multiple attachments are specified in mapForDraw()
         * / mapDefaultForDraw(), the data are written to each of them.
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         */
        inline static void blit(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLint>& topRight, const Math::Vector2<GLint>& destinationBottomLeft, const Math::Vector2<GLint>& destinationTopRight, BlitMask blitMask, AbstractTexture::Filter filter) {
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), destinationBottomLeft.x(), destinationBottomLeft.y(), destinationTopRight.x(), destinationTopRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(filter));
        }

        /**
         * @brief Copy block of pixels from read to draw framebuffer
         * @param bottomLeft        Bottom left coordinates of source and
         *      destination rectangle
         * @param topRight          Top right coordinates of source and
         *      destination rectangle
         * @param blitMask          Blit mask
         *
         * Convenience function when source rectangle is the same as
         * destination rectangle. As the image is copied pixel-by-pixel,
         * no interpolation is needed and thus
         * AbstractTexture::Filter::NearestNeighbor filtering is used by
         * default.
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         */
        inline static void blit(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLint>& topRight, BlitMask blitMask) {
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(AbstractTexture::Filter::NearestNeighbor));
        }
        #endif

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param offset            Offset in the framebuffer
         * @param dimensions        Image dimensions
         * @param components        Color components
         * @param type              Data type
         * @param image             %Image where to put the data
         *
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& dimensions, AbstractImage::Components components, AbstractImage::ComponentType type, Image2D* image);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read block of pixels from framebuffer to buffered image
         * @param offset            Offset in the framebuffer
         * @param dimensions        Image dimensions
         * @param components        Color components
         * @param type              Data type
         * @param image             Buffered image where to put the data
         * @param usage             %Buffer usage
         *
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& dimensions, AbstractImage::Components components, AbstractImage::ComponentType type, BufferedImage2D* image, Buffer::Usage usage);
        #endif

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline Framebuffer() { glGenFramebuffers(1, &framebuffer); }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline ~Framebuffer() { glDeleteFramebuffers(1, &framebuffer); }

        /**
         * @brief Bind framebuffer
         *
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void bind(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Map given color attachments of current framebuffer for drawing
         * @param colorAttachments  Color attachment IDs. If any value is -1,
         *      given output is not used.
         *
         * If used for mapping output of fragment shader, the order must be as
         * specified by the shader (see AbstractShaderProgram documentation).
         * If used for blit(), the order is not important. Each used attachment
         * should have either renderbuffer or texture attached for writing to
         * work properly.
         * @see mapDefaultForDraw(), mapForRead()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        void mapForDraw(std::initializer_list<int> colorAttachments);

        /**
         * @brief Map given color attachment of current framebuffer for reading
         * @param colorAttachment   Color attachment ID
         *
         * The color attachment should have either renderbuffer or texture
         * attached for reading to work properly.
         * @see mapDefaultForRead(), mapForDraw()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void mapForRead(unsigned int colorAttachment) {
            bind(Target::Read);
            glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
        }
        #endif

        /**
         * @brief Attach renderbuffer to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param renderbuffer      Renderbuffer
         *
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachRenderbuffer(Target target, DepthStencilAttachment depthStencilAttachment, Renderbuffer* renderbuffer) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), GL_RENDERBUFFER, renderbuffer->id());
        }

        /**
         * @brief Attach renderbuffer to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param renderbuffer      Renderbuffer
         *
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachRenderbuffer(Target target, unsigned int colorAttachment, Renderbuffer* renderbuffer) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, GL_RENDERBUFFER, renderbuffer->id());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 1D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture1D(Target target, DepthStencilAttachment depthStencilAttachment, Texture1D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture1D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 1D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture1D(Target target, unsigned int colorAttachment, Texture1D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture1D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }
        #endif

        /**
         * @brief Attach 2D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * @see attachCubeMapTexture()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture2D(Target target, DepthStencilAttachment depthStencilAttachment, Texture2D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 2D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * @see attachCubeMapTexture()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture2D(Target target, unsigned int colorAttachment, Texture2D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach cube map texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * @see attachTexture2D()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachCubeMapTexture(Target target, DepthStencilAttachment depthStencilAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(coordinate), texture->id(), mipLevel);
        }

        /**
         * @brief Attach cube map texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * @see attachTexture2D()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachCubeMapTexture(Target target, unsigned int colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(coordinate), texture->id(), mipLevel);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 3D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture
         *
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture3D(Target target, DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture3D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
        }

        /**
         * @brief Attach 3D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture.
         *
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture3D(Target target, unsigned int colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture3D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
        }
        #endif

    private:
        static ClearMask clearMask;

        GLuint framebuffer;
};

}

#endif
