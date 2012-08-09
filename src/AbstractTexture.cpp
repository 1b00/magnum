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

#include "AbstractTexture.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT

/* Check correctness of binary OR in setMinificationFilter(). If nobody fucks
   anything up, this assert should produce the same results on all dimensions,
   thus testing only on AbstractTexture. */
#define filter_or(filter, mipmap) \
    (static_cast<GLint>(AbstractTexture::Filter::filter)|static_cast<GLint>(AbstractTexture::Mipmap::mipmap))
static_assert((filter_or(NearestNeighbor, BaseLevel) == GL_NEAREST) &&
              (filter_or(NearestNeighbor, NearestLevel) == GL_NEAREST_MIPMAP_NEAREST) &&
              (filter_or(NearestNeighbor, LinearInterpolation) == GL_NEAREST_MIPMAP_LINEAR) &&
              (filter_or(LinearInterpolation, BaseLevel) == GL_LINEAR) &&
              (filter_or(LinearInterpolation, NearestLevel) == GL_LINEAR_MIPMAP_NEAREST) &&
              (filter_or(LinearInterpolation, LinearInterpolation) == GL_LINEAR_MIPMAP_LINEAR),
    "Unsupported constants for GL texture filtering");
#undef filter_or
#endif

GLint AbstractTexture::maxSupportedLayerCount() {
    GLint value;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
    return value;
}

#ifndef MAGNUM_TARGET_GLES
GLfloat AbstractTexture::maxSupportedAnisotropy() {
    GLfloat value;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    return value;
}
#endif

void AbstractTexture::setMinificationFilter(Filter filter, Mipmap mipmap) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Mipmap::BaseLevel, "AbstractTexture: rectangle textures cannot have mipmaps", );
    #endif

    bind();
    glTexParameteri(_target, GL_TEXTURE_MIN_FILTER,
        static_cast<GLint>(filter)|static_cast<GLint>(mipmap));
}

void AbstractTexture::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE, "AbstractTexture: rectangle textures cannot have mipmaps", );
    #endif

    bind();
    glGenerateMipmap(_target);
}

#ifndef MAGNUM_TARGET_GLES
AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components components, AbstractTexture::ComponentType type) {
    #define internalFormatSwitch(c) switch(type) {                          \
        case ComponentType::UnsignedByte:                                   \
            internalFormat = GL_##c##8UI; break;                            \
        case ComponentType::Byte:                                           \
            internalFormat = GL_##c##8I; break;                             \
        case ComponentType::UnsignedShort:                                  \
            internalFormat = GL_##c##16UI; break;                           \
        case ComponentType::Short:                                          \
            internalFormat = GL_##c##16I; break;                            \
        case ComponentType::UnsignedInt:                                    \
            internalFormat = GL_##c##32UI; break;                           \
        case ComponentType::Int:                                            \
            internalFormat = GL_##c##32I; break;                            \
        case ComponentType::Half:                                           \
            internalFormat = GL_##c##16F; break;                            \
        case ComponentType::Float:                                          \
            internalFormat = GL_##c##32F; break;                            \
        case ComponentType::NormalizedUnsignedByte:                         \
            internalFormat = GL_##c##8; break;                              \
        case ComponentType::NormalizedByte:                                 \
            internalFormat = GL_##c##8_SNORM; break;                        \
        case ComponentType::NormalizedUnsignedShort:                        \
            internalFormat = GL_##c##16; break;                             \
        case ComponentType::NormalizedShort:                                \
            internalFormat = GL_##c##16_SNORM; break;                       \
    }
    if(components == Components::Red)
        internalFormatSwitch(R)
    else if(components == Components::RedGreen)
        internalFormatSwitch(RG)
    else if(components == Components::RGB)
        internalFormatSwitch(RGB)
    else if(components == Components::RGBA)
        internalFormatSwitch(RGBA)
    #undef internalFormatSwitch
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractTexture::DataHelper<2>::setWrapping(GLenum target, const Math::Vector<2, Wrapping>& wrapping) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(target != GL_TEXTURE_RECTANGLE || ((wrapping[0] == Wrapping::ClampToEdge || wrapping[0] == Wrapping::ClampToBorder) && (wrapping[0] == Wrapping::ClampToEdge || wrapping[1] == Wrapping::ClampToEdge)), "AbstractTexture: rectangle texture wrapping must either clamp to border or to edge", );
    #endif

    glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
}

void AbstractTexture::DataHelper<3>::setWrapping(GLenum target, const Math::Vector<3, Wrapping>& wrapping) {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[1]));
    #ifndef MAGNUM_TARGET_GLES
    glTexParameteri(target, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping[2]));
    #endif
}
#endif

}
