#ifndef Magnum_Shaders_Shader_h
#define Magnum_Shaders_Shader_h
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
 * @brief Forward declarations for Magnum::Shaders namespace
 */

#include <cstdint>

namespace Magnum { namespace Shaders {

template<std::uint8_t> class FlatShader;
typedef FlatShader<2> FlatShader2D;
typedef FlatShader<3> FlatShader3D;

class PhongShader;

template<std::uint8_t> class VertexColorShader;
typedef VertexColorShader<2> VertexColorShader2D;
typedef VertexColorShader<3> VertexColorShader3D;

}}

#endif
