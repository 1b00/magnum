#ifndef Magnum_CubeMapTexture_h
#define Magnum_CubeMapTexture_h
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

/** @file
 * @brief Class Magnum::CubeMapTexture
 */

#include "AbstractTexture.h"

namespace Magnum {

/**
@brief Cube map texture

%Texture used mainly for environment maps. It consists of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

@section CubeMapTexture-usage Basic usage

See Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data from e.g. set of Image objects:
@code
Image2D positiveX({256, 256}, Image2D::Components::RGBA, Image2D::ComponentType::UnsignedByte, dataPositiveX);
// ...

CubeMapTexture texture;
texture.setMagnificationFilter(Texture2D::Filter::Linear)
    // ...
    ->setStorage(Math::log2(256)+1, Texture2D::Format::RGBA8, {256, 256})
    ->setSubImage(CubeMapTexture::Coordinate::PositiveX, 0, {}, &positiveX)
    ->setSubImage(CubeMapTexture::Coordinate::NegativeX, 0, {}, &negativeX)
    // ...
@endcode

The texture is bound to layer specified by shader via bind(). In shader, the
texture is used via `samplerCube`. Unlike in classic textures, coordinates for
cube map textures is signed three-part vector from the center of the cube,
which intersects one of the six sides of the cube map. See also
AbstractShaderProgram for more information.

@see CubeMapTextureArray
*/
class CubeMapTexture: public AbstractTexture {
    public:
        /** @brief Cube map coordinate */
        enum Coordinate: GLenum {
            PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,     /**< +X cube side */
            NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,     /**< -X cube side */
            PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,     /**< +Y cube side */
            NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,     /**< -Y cube side */
            PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,     /**< +Z cube side */
            NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z      /**< -Z cube side */
        };

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Enable/disable seamless cube map textures
         *
         * Initially disabled on desktop OpenGL.
         * @see @fn_gl{Enable}/@fn_gl{Disable} with @def_gl{TEXTURE_CUBE_MAP_SEAMLESS}
         * @requires_gl32 %Extension @extension{ARB,seamless_cube_map}
         * @requires_gl Not available in OpenGL ES 2.0, always enabled in
         *      OpenGL ES 3.0.
         */
        inline static void setSeamless(bool enabled) {
            enabled ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
        #endif

        /**
         * @brief Constructor
         *
         * Creates one cube map OpenGL texture.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_CUBE_MAP}
         */
        inline explicit CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        /**
         * @brief Set wrapping
         *
         * See Texture::setWrapping() for more information.
         */
        inline CubeMapTexture* setWrapping(const Array3D<Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(this, wrapping);
            return this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief %Image size in given mip level
         * @param coordinate        Coordinate
         * @param level             Mip level
         *
         * See Texture::imageSize() for more information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        inline Vector2i imageSize(Coordinate coordinate, Int level) {
            return DataHelper<2>::imageSize(this, static_cast<GLenum>(coordinate), level);
        }
        #endif

        /**
         * @brief Set storage
         *
         * See Texture::setStorage() for more information.
         */
        inline CubeMapTexture* setStorage(Int levels, InternalFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(this, _target, levels, internalFormat, size);
            return this;
        }

        /**
         * @brief Set image data
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             Image, ImageWrapper, BufferImage or
         *      Trade::ImageData of the same dimension count
         * @return Pointer to self (for method chaining)
         *
         * See Texture::setImage() for more information.
         */
        template<class Image> inline CubeMapTexture* setImage(Coordinate coordinate, Int level, InternalFormat internalFormat, Image* image) {
            DataHelper<2>::set(this, static_cast<GLenum>(coordinate), level, internalFormat, image);
            return this;
        }

        /**
         * @brief Set image subdata
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             Image, ImageWrapper, BufferImage or
         *      Trade::ImageData of the same or one less dimension count
         * @return Pointer to self (for method chaining)
         *
         * See Texture::setSubImage() for more information.
         */
        template<class Image> inline CubeMapTexture* setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, const Image* image) {
            DataHelper<2>::setSub(this, static_cast<GLenum>(coordinate), level, offset, image);
            return this;
        }

        /**
         * @brief Invalidate texture subimage
         * @param level             Mip level
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * Z coordinate is equivalent to number of texture face, i.e.
         * @ref Coordinate "Coordinate::PositiveX" is `0` and so on, in the
         * same order as in the enum.
         *
         * See Texture::invalidateSubImage() for more information.
         */
        inline void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSub(this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline CubeMapTexture* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return this;
        }
        inline CubeMapTexture* setMagnificationFilter(Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return this;
        }
        #ifndef MAGNUM_TARGET_GLES
        inline CubeMapTexture* setBorderColor(const Color4<>& color) {
            AbstractTexture::setBorderColor(color);
            return this;
        }
        #endif
        inline CubeMapTexture* setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return this;
        }
        inline CubeMapTexture* generateMipmap() {
            AbstractTexture::generateMipmap();
            return this;
        }
        #endif
};

}

#endif
