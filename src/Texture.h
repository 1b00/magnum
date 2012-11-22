#ifndef Magnum_Texture_h
#define Magnum_Texture_h
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
 * @brief Class Magnum::Texture, typedef Magnum::Texture1D, Magnum::Texture2D, Magnum::Texture3D
 */

#include "AbstractTexture.h"
#include "DimensionTraits.h"

namespace Magnum {

/**
@brief %Texture

Template class for one- to three-dimensional textures. See AbstractTexture
documentation for more information.

In shader, the texture is used via `sampler1D`, `sampler2D` or `sampler3D`
depending on dimension count. Note that you can have more than one texture bound
to the shader - the only requirement is to have each texture in another layer.

@section RectangleTextures Rectangle textures

If you want to use rectangle textures, set target in constructor to
`Target::Rectangle` and in shader use `sampler2DRect`. Unlike `sampler2D`,
which accepts coordinates between 0 and 1, `sampler2DRect` accepts coordinates
between 0 and `textureSizeInGivenDirection-1`. Note that rectangle textures
don't support mipmapping and repeating wrapping modes, see @ref Texture::Filter
"Filter", @ref Texture::Mipmap "Mipmap" and generateMipmap() documentation
for more information.

@requires_gl Rectangle textures are not available in OpenGL ES.
@requires_gl31 Extension @extension{ARB,texture_rectangle} (rectangle textures)

@see Texture1D, Texture2D, Texture3D, CubeMapTexture, CubeMapTextureArray
@todo @extension{AMD,sparse_texture}
 */
template<std::uint8_t dimensions> class Texture: public AbstractTexture {
    public:
        static const std::uint8_t Dimensions = dimensions; /**< @brief %Texture dimension count */

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief %Texture target
         *
         * Each dimension has its own unique subset of these targets.
         */
        enum class Target: GLenum {
            /**
             * One-dimensional texture
             * @requires_gl Only 2D and 3D textures are available in OpenGL
             *      ES.
             */
            Texture1D = GL_TEXTURE_1D,

            Texture2D = GL_TEXTURE_2D, /**< Two-dimensional texture */

            /**
             * Three-dimensional texture
             * @requires_gles30 %Extension @es_extension{OES,texture_3D}
             */
            Texture3D = GL_TEXTURE_3D,

            /**
             * One-dimensional texture array (i.e. two dimensions in total)
             * @requires_gl30 Extension @extension{EXT,texture_array}
             * @requires_gl Only 2D and 3D textures are available in OpenGL
             *      ES.
             */
            Texture1DArray = GL_TEXTURE_1D_ARRAY,

            /**
             * Two-dimensional texture array (i.e. three dimensions in total)
             * @requires_gl30 Extension @extension{EXT,texture_array}
             * @requires_gles30 Array textures are not available in OpenGL ES
             *      2.0.
             */
            Texture2DArray = GL_TEXTURE_2D_ARRAY,

            /**
             * Rectangle texture (i.e. two dimensions)
             * @requires_gl31 Extension @extension{ARB,texture_rectangle}
             * @requires_gl Rectangle textures are not available in OpenGL ES.
             */
            Rectangle = GL_TEXTURE_RECTANGLE
        };
        #else
        typedef typename DataHelper<Dimensions>::Target Target; /**< @brief %Texture target */
        #endif

        /**
         * @brief Constructor
         * @param target            %Texture target. If not set, default value
         *      is `Target::Texture1D`, `Target::Texture2D` or
         *      `Target::Texture3D` based on dimension count.
         *
         * Creates one OpenGL texture.
         */
        inline Texture(Target target = DataHelper<Dimensions>::target()): AbstractTexture(static_cast<GLenum>(target)) {}

        /** @brief %Texture target */
        inline constexpr Target target() const { return static_cast<Target>(_target); }

        /**
         * @brief Set wrapping
         * @param wrapping          Wrapping type for all texture dimensions
         * @return Pointer to self (for method chaining)
         *
         * Sets wrapping type for coordinates out of range (0, 1) for normal
         * textures and (0, textureSizeInGivenDirection-1) for rectangle
         * textures. If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some layer before the operation.
         * @attention For rectangle textures only some modes are supported,
         *      see @ref AbstractTexture::Wrapping "Wrapping" documentation
         *      for more information.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WRAP_S}, @def_gl{TEXTURE_WRAP_T},
         *      @def_gl{TEXTURE_WRAP_R}
         * @todo Use something better for this than Vector (mainly something
         *      that can easily create all values the same)
         */
        inline Texture<Dimensions>* setWrapping(const Math::Vector<Dimensions, Wrapping>& wrapping) {
            DataHelper<Dimensions>::setWrapping(this, wrapping);
            return this;
        }

        /**
         * @brief Set texture data
         * @param mipLevel          Mip level
         * @param internalFormat    Internal texture format
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData of the same dimension count
         * @return Pointer to self (for method chaining)
         *
         * Sets texture data from given image. The image is not deleted
         * afterwards. If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some layer before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexImage1D}/
         *      @fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}
         */
        template<class Image> inline Texture<Dimensions>* setData(GLint mipLevel, InternalFormat internalFormat, Image* image) {
            DataHelper<Dimensions>::set(this, _target, mipLevel, internalFormat, image);
            return this;
        }

        /**
         * @brief Set texture subdata
         * @param mipLevel          Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData
         * @return Pointer to self (for method chaining)
         *
         * Sets texture subdata from given image. The image is not deleted
         * afterwards. The image can have either the same dimension count or
         * have one dimension less, but at least one dimension.
         *
         * If the image has one dimension less than the texture, the image is
         * taken as if it had the last dimension equal to 1. It can be used
         * for e.g. updating 3D texture with multiple 2D images or for filling
         * 1D texture array (which is two-dimensional) with 1D images.
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some layer before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexSubImage1D}/
         *      @fn_gl{TexSubImage2D}/@fn_gl{TexSubImage3D} or
         *      @fn_gl_extension{TextureSubImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access}
         */
        template<class Image> inline Texture<Dimensions>* setSubData(GLint mipLevel, const typename DimensionTraits<Dimensions, GLint>::VectorType& offset, Image* image) {
            DataHelper<Dimensions>::setSub(this, _target, mipLevel, offset, image);
            return this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Texture<Dimensions>* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return this;
        }
        inline Texture<Dimensions>* setMagnificationFilter(Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return this;
        }
        #ifndef MAGNUM_TARGET_GLES
        inline Texture<Dimensions>* setBorderColor(const Color4<>& color) {
            AbstractTexture::setBorderColor(color);
            return this;
        }
        inline Texture<Dimensions>* setMaxAnisotropy(GLfloat anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return this;
        }
        #endif
        inline Texture<Dimensions>* generateMipmap() {
            AbstractTexture::generateMipmap();
            return this;
        }
        #endif
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture

@requires_gl Only 2D and 3D textures are available in OpenGL ES.
*/
typedef Texture<1> Texture1D;
#endif

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

/**
@brief Three-dimensional texture

@requires_gles30 %Extension @es_extension{OES,texture_3D}
*/
typedef Texture<3> Texture3D;

}

#endif
