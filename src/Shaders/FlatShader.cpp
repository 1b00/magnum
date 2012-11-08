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

#include "FlatShader.h"

#include <Utility/Resource.h>

#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<std::uint8_t dimensions> struct ShaderName {};

    template<> struct ShaderName<2> {
        constexpr static const char* Vertex = "FlatShader2D.vert";
        constexpr static const char* Fragment = "FlatShader2D.frag";
    };

    template<> struct ShaderName<3> {
        constexpr static const char* Vertex = "FlatShader3D.vert";
        constexpr static const char* Fragment = "FlatShader3D.frag";
    };
}

template<std::uint8_t dimensions> FlatShader<dimensions>::FlatShader() {
    Corrade::Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    Version v = Context::current()->supportedVersion({/*Version::GL320, */Version::GL210});
    #else
    Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vertexShader(v, Shader::Type::Vertex);
    vertexShader.addSource(rs.get("compatibility.glsl"));
    vertexShader.addSource(rs.get(ShaderName<dimensions>::Vertex));
    attachShader(vertexShader);

    Shader fragmentShader(v, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get(ShaderName<dimensions>::Fragment));
    attachShader(fragmentShader);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>()) {
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300)) {
    #endif
        bindAttributeLocation(Position::Location, "position");
    }

    link();

    transformationProjectionUniform = uniformLocation("transformationProjection");
    colorUniform = uniformLocation("color");
}

template class FlatShader<2>;
template class FlatShader<3>;

}}
