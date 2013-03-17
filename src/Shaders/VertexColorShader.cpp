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

#include "VertexColorShader.h"

#include <Utility/Resource.h>

#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<UnsignedInt> constexpr const char* vertexShaderName();
    template<> constexpr const char* vertexShaderName<2>() { return "VertexColorShader2D.vert"; }
    template<> constexpr const char* vertexShaderName<3>() { return "VertexColorShader3D.vert"; }
}

template<UnsignedInt dimensions> VertexColorShader<dimensions>::VertexColorShader(): transformationProjectionMatrixUniform(0) {
    Corrade::Utility::Resource rs("MagnumShaders");

    /* Weird bug in GCC 4.5 - cannot use initializer list here, although the
       same thing works in PhongShader flawlessly*/
    #ifndef CORRADE_GCC45_COMPATIBILITY
    #ifndef MAGNUM_TARGET_GLES
    Version v = Context::current()->supportedVersion({Version::GL320, Version::GL210});
    #else
    Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif
    #else
    #ifndef MAGNUM_TARGET_GLES
    std::initializer_list<Version> vs{Version::GL320, Version::GL210};
    #else
    std::initializer_list<Version> vs{Version::GLES300, Version::GLES200};
    #endif
    Version v = Context::current()->supportedVersion(vs);
    #endif

    Shader vertexShader(v, Shader::Type::Vertex);
    vertexShader.addSource(rs.get("compatibility.glsl"));
    vertexShader.addSource(rs.get(vertexShaderName<dimensions>()));
    attachShader(vertexShader);

    Shader fragmentShader(v, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get("VertexColorShader.frag"));
    attachShader(fragmentShader);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>() ||
        Context::current()->version() == Version::GL210) {
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300)) {
    #endif
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Color::Location, "color");
    }

    link();

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>()) {
    #else
    {
    #endif
        transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
    }
}

template class VertexColorShader<2>;
template class VertexColorShader<3>;

}}
