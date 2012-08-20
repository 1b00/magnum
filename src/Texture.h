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

namespace Magnum {

/**
@brief %Texture

Template class for one- to three-dimensional textures. See AbstractTexture
documentation for more information about usage.

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

@requires_gl (rectangle textures)
@requires_gl31 Extension @extension{ARB,texture_rectangle} (rectangle textures)

@see CubeMapTexture, CubeMapTextureArray
 */
template<size_t textureDimensions> class Texture: public AbstractTexture {
    public:
        static const size_t Dimensions = textureDimensions; /**< @brief %Texture dimension count */

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief %Texture target
         *
         * Each dimension has its own unique subset of these targets.
         */
        enum class Target: GLenum {
            /**
             * One-dimensional texture
             * @requires_gl
             */
            Texture1D = GL_TEXTURE_1D,

            Texture2D = GL_TEXTURE_2D, /**< Two-dimensional texture */

            /**
             * Three-dimensional texture
             * @requires_gl
             */
            Texture3D = GL_TEXTURE_3D,

            /**
             * One-dimensional texture array (i.e. two dimensions in total)
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_array}
             */
            Texture1DArray = GL_TEXTURE_1D_ARRAY,

            /**
             * Two-dimensional texture array (i.e. three dimensions in total)
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_array}
             */
            Texture2DArray = GL_TEXTURE_2D_ARRAY,

            /**
             * Rectangle texture (i.e. two dimensions)
             * @requires_gl
             * @requires_gl31 Extension @extension{ARB,texture_rectangle}
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
         *
         * Sets wrapping type for coordinates out of range (0, 1) for normal
         * textures and (0, textureSizeInGivenDirection-1) for rectangle
         * textures.
         * @attention For rectangle textures only some modes are supported,
         * see @ref AbstractTexture::Wrapping "Wrapping" documentation for
         * more information.
         */
        inline void setWrapping(const Math::Vector<Dimensions, Wrapping>& wrapping) {
            bind();
            DataHelper<Dimensions>::setWrapping(_target, wrapping);
        }

        /**
         * @brief Set texture data
         * @param mipLevel          Mip level
         * @param internalFormat    Internal texture format
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData of the same dimension count
         *
         * Sets texture data from given image. The image is not deleted
         * afterwards.
         */
        template<class Image> inline void setData(GLint mipLevel, InternalFormat internalFormat, Image* image) {
            bind();
            DataHelper<Dimensions>::set(_target, mipLevel, internalFormat, image);
        }

        /**
         * @brief Set texture subdata
         * @param mipLevel          Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData
         *
         * Sets texture subdata from given image. The image is not deleted
         * afterwards. The image can have either the same dimension count or
         * have one dimension less, but at least one dimension.
         *
         * If the image has one dimension less than the texture, the image is
         * taken as if it had the last dimension equal to 1. It can be used
         * for e.g. updating 3D texture with multiple 2D images or for filling
         * 1D texture array (which is two-dimensional) with 1D images.
         */
        template<class Image> inline void setSubData(GLint mipLevel, const Math::Vector<Dimensions, GLint>& offset, Image* image) {
            bind();
            DataHelper<Dimensions>::setSub(_target, mipLevel, offset, image);
        }
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture

@requires_gl
*/
typedef Texture<1> Texture1D;
#endif

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

#ifndef MAGNUM_TARGET_GLES
/**
@brief Three-dimensional texture

@requires_gl
*/
typedef Texture<3> Texture3D;
#endif

}

#endif
