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

#include "BufferedTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"

namespace Magnum {

BufferedTexture::SetBufferImplementation BufferedTexture::setBufferImplementation = &BufferedTexture::setBufferImplementationDefault;

void BufferedTexture::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "BufferedTexture: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        setBufferImplementation = &BufferedTexture::setBufferImplementationDSA;
    }
}

void BufferedTexture::setBufferImplementationDefault(BufferedTexture::InternalFormat internalFormat, Buffer* buffer) {
    bindInternal();
    glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buffer->id());
}

void BufferedTexture::setBufferImplementationDSA(BufferedTexture::InternalFormat internalFormat, Buffer* buffer) {
    glTextureBufferEXT(id(), GL_TEXTURE_BUFFER, internalFormat, buffer->id());
}

BufferedTexture::InternalFormat::InternalFormat(Components components, ComponentType type) {
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
        case ComponentType::NormalizedUnsignedShort:                        \
            internalFormat = GL_##c##16; break;                             \
    }
    if(components == Components::Red)
        internalFormatSwitch(R)
    else if(components == Components::RedGreen)
        internalFormatSwitch(RG)
    else if(components == Components::RGBA)
        internalFormatSwitch(RGBA)
    #undef internalFormatSwitch
}

}
#endif
