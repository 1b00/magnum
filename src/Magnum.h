#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
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
 * @brief Forward declarations for Magnum namespace
 */

#include <Utility/Utility.h>

#include "Math/Math.h"
#include "Types.h"
#include "magnumConfigure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef unsigned int GLenum; /* Needed for *Format and *Type enums */
#endif

namespace Magnum {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Math {
    template<class T> struct Constants;

    #ifndef CORRADE_GCC46_COMPATIBILITY
    #ifndef MAGNUM_TARGET_GLES
    constexpr Rad<Double> operator "" _rad(long double);
    constexpr Deg<Double> operator "" _deg(long double);
    #endif
    constexpr Rad<Float> operator "" _radf(long double);
    constexpr Deg<Float> operator "" _degf(long double);
    #endif
}

/* Bring whole Corrade namespace */
using namespace Corrade;

/* Bring debugging facility from Corrade::Utility namespace */
using Corrade::Utility::Debug;
using Corrade::Utility::Warning;
using Corrade::Utility::Error;
#endif

#ifdef DOXYGEN_GENERATING_OUTPUT

/** @todoc remove trailing underscores when Doxygen can handle `undef` */

/**
@brief Static library build

`MAGNUM_BUILD_STATIC` is defined if build as static libraries. Default are
shared libraries.
@see @ref building-corrade
*/
#define MAGNUM_BUILD_STATIC_

/**
@brief OpenGL ES target

`MAGNUM_TARGET_GLES` is defined if the engine is built for OpenGL ES 3.0 or
OpenGL ES 2.0.
@see @ref MAGNUM_TARGET_GLES2_ "MAGNUM_TARGET_GLES2",
    @ref MAGNUM_TARGET_GLES3_ "MAGNUM_TARGET_GLES3",
    @ref MAGNUM_TARGET_DESKTOP_GLES_ "MAGNUM_TARGET_DESKTOP_GLES",
    @ref building
*/
#define MAGNUM_TARGET_GLES_

/**
@brief OpenGL ES 2.0 target.

`MAGNUM_TARGET_GLES2` is defined if the engine is built for OpenGL ES 2.0.
Implies also @ref MAGNUM_TARGET_GLES_ "MAGNUM_TARGET_GLES".
@see @ref MAGNUM_TARGET_GLES3_ "MAGNUM_TARGET_GLES3",
    @ref MAGNUM_TARGET_DESKTOP_GLES_ "MAGNUM_TARGET_DESKTOP_GLES",
    @ref building
*/
#define MAGNUM_TARGET_GLES2_

/**
@brief OpenGL ES 3.0 target.

`MAGNUM_TARGET_GLES3` is defined if the engine is built for OpenGL ES 3.0.
Implies also @ref MAGNUM_TARGET_GLES_ "MAGNUM_TARGET_GLES".
@see @ref MAGNUM_TARGET_GLES2_ "MAGNUM_TARGET_GLES2",
    @ref MAGNUM_TARGET_DESKTOP_GLES_ "MAGNUM_TARGET_DESKTOP_GLES",
    @ref building
*/
#define MAGNUM_TARGET_GLES3_

/**
@brief Desktop emulation of OpenGL ES target

`MAGNUM_TARGET_DESKTOP_GLES` is defined if the engine is built for OpenGL ES
3.0 or OpenGL ES 2.0 emulated within standard desktop OpenGL. Implies also
@ref MAGNUM_TARGET_GLES_ "MAGNUM_TARGET_GLES".
@see @ref MAGNUM_TARGET_GLES2_ "MAGNUM_TARGET_GLES2", @ref building
*/
#define MAGNUM_TARGET_DESKTOP_GLES_

#endif

/** @{ @name Basic type definitions

See @ref types for more information.
*/

/** @brief Unsigned byte (8bit) */
typedef std::uint8_t UnsignedByte;

/** @brief Signed byte (8bit) */
typedef std::int8_t Byte;

/** @brief Unsigned short (16bit) */
typedef std::uint16_t UnsignedShort;

/** @brief Signed short (16bit) */
typedef std::int16_t Short;

/** @brief Unsigned int (32bit) */
typedef std::uint32_t UnsignedInt;

/** @brief Signed int (32bit) */
typedef std::int32_t Int;

/** @brief Unsigned long (64bit) */
typedef std::uint64_t UnsignedLong;

/** @brief Signed long (64bit) */
typedef std::int64_t Long;

/** @brief Float (32bit) */
typedef float Float;

/** @brief Two-component float vector */
typedef Math::Vector2<Float> Vector2;

/** @brief Three-component float vector */
typedef Math::Vector3<Float> Vector3;

/** @brief Four-component float vector */
typedef Math::Vector4<Float> Vector4;

/** @brief Two-component unsigned integer vector */
typedef Math::Vector2<UnsignedInt> Vector2ui;

/** @brief Three-component unsigned integer vector */
typedef Math::Vector3<UnsignedInt> Vector3ui;

/** @brief Four-component unsigned integer vector */
typedef Math::Vector4<UnsignedInt> Vector4ui;

/** @brief Two-component signed integer vector */
typedef Math::Vector2<Int> Vector2i;

/** @brief Three-component signed integer vector */
typedef Math::Vector3<Int> Vector3i;

/** @brief Four-component signed integer vector */
typedef Math::Vector4<Int> Vector4i;

/** @brief 2x2 float matrix */
typedef Math::Matrix<2, Float> Matrix2;

/** @brief 3x3 float matrix */
typedef Math::Matrix3<Float> Matrix3;

/** @brief 4x4 float matrix */
typedef Math::Matrix4<Float> Matrix4;

/** @brief Float matrix with 2 columns and 3 rows */
typedef Math::RectangularMatrix<2, 3, Float> Matrix2x3;

/** @brief Float matrix with 3 columns and 2 rows */
typedef Math::RectangularMatrix<3, 2, Float> Matrix3x2;

/** @brief Float matrix with 2 columns and 4 rows */
typedef Math::RectangularMatrix<2, 4, Float> Matrix2x4;

/** @brief Float matrix with 4 columns and 2 rows */
typedef Math::RectangularMatrix<4, 2, Float> Matrix4x2;

/** @brief Float matrix with 3 columns and 4 rows */
typedef Math::RectangularMatrix<3, 4, Float> Matrix3x4;

/** @brief Float matrix with 4 columns and 3 rows */
typedef Math::RectangularMatrix<4, 3, Float> Matrix4x3;

/** @brief Float complex number */
typedef Math::Complex<Float> Complex;

/** @brief Float dual complex number */
typedef Math::DualComplex<Float> DualComplex;

/** @brief Float quaternion */
typedef Math::Quaternion<Float> Quaternion;

/** @brief Float dual quaternion */
typedef Math::DualQuaternion<Float> DualQuaternion;

/** @brief Float constants */
typedef Math::Constants<Float> Constants;

/** @brief Angle in float degrees */
typedef Math::Deg<Float> Deg;

/** @brief Angle in float radians */
typedef Math::Rad<Float> Rad;

/** @brief Float rectangle */
typedef Math::Geometry::Rectangle<Float> Rectangle;

/** @brief Signed integer rectangle */
typedef Math::Geometry::Rectangle<Int> Rectanglei;

/*@}*/

#ifndef MAGNUM_TARGET_GLES
/** @{ @name Double-precision types

See @ref types for more information.
@requires_gl Only single-precision types are available in OpenGL ES.
*/

/** @brief Double (64bit) */
typedef double Double;

/** @brief Two-component double vector */
typedef Math::Vector2<Double> Vector2d;

/** @brief Three-component double vector */
typedef Math::Vector3<Double> Vector3d;

/** @brief Four-component double vector */
typedef Math::Vector4<Double> Vector4d;

/** @brief 2x2 double matrix */
typedef Math::Matrix<2, Double> Matrix2d;

/** @brief 3x3 double matrix */
typedef Math::Matrix3<Double> Matrix3d;

/** @brief 4x4 double matrix */
typedef Math::Matrix4<Double> Matrix4d;

/** @brief Double matrix with 2 columns and 3 rows */
typedef Math::RectangularMatrix<2, 3, Double> Matrix2x3d;

/** @brief Double matrix with 3 columns and 2 rows */
typedef Math::RectangularMatrix<3, 2, Double> Matrix3x2d;

/** @brief Double matrix with 2 columns and 4 rows */
typedef Math::RectangularMatrix<2, 4, Double> Matrix2x4d;

/** @brief Double matrix with 4 columns and 2 rows */
typedef Math::RectangularMatrix<4, 2, Double> Matrix4x2d;

/** @brief Double matrix with 3 columns and 4 rows */
typedef Math::RectangularMatrix<3, 4, Double> Matrix3x4d;

/** @brief Double matrix with 4 columns and 3 rows */
typedef Math::RectangularMatrix<4, 3, Double> Matrix4x3d;

/** @brief Double complex number */
typedef Math::Complex<Double> Complexd;

/** @brief Double dual complex number */
typedef Math::DualComplex<Double> DualComplexd;

/** @brief Double quaternion */
typedef Math::Quaternion<Double> Quaterniond;

/** @brief Double dual quaternion */
typedef Math::DualQuaternion<Double> DualQuaterniond;

/** @brief Double constants */
typedef Math::Constants<Double> Constantsd;

/** @brief Angle in double degrees */
typedef Math::Deg<Double> Degd;

/** @brief Angle in double radians */
typedef Math::Rad<Double> Radd;

/** @brief Double rectangle */
typedef Math::Geometry::Rectangle<Double> Rectangled;

/*@}*/
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/* Using angle literals from Math namespace */
#ifndef MAGNUM_TARGET_GLES
using Math::operator "" _deg;
using Math::operator "" _rad;
#endif
using Math::operator "" _degf;
using Math::operator "" _radf;
#endif

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
/* Forward declarations for all types in root namespace */

/* FramebufferClear[Mask], FramebufferBlit[Mask], FramebufferBlitFilter,
   FramebufferTarget enums used only directly with framebuffer instance */
class AbstractFramebuffer;

class AbstractImage;
class AbstractShaderProgram;
class AbstractTexture;

template<UnsignedInt, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

class Buffer;

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt> class BufferImage;
typedef BufferImage<1> BufferImage1D;
typedef BufferImage<2> BufferImage2D;
typedef BufferImage<3> BufferImage3D;
#endif

#ifndef MAGNUM_TARGET_GLES
class BufferTexture;
#ifndef CORRADE_GCC45_COMPATIBILITY
enum class BufferTextureFormat: GLenum;
#endif
#endif

template<class T = Float> class Color3;
template<class T = Float> class Color4;

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class Version: Int;
#endif
class Context;
class CubeMapTexture;

#ifndef MAGNUM_TARGET_GLES
class CubeMapTextureArray;
#endif

/* DebugMarker forward declaration is not needed */
/* DefaultFramebuffer is available only through global instance */
/* DimensionTraits forward declaration is not needed */

class Extension;
class Framebuffer;

template<UnsignedInt> class Image;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class ImageFormat: GLenum;
enum class ImageType: GLenum;
#endif

template<UnsignedInt> class ImageReference;
typedef ImageReference<1> ImageReference1D;
typedef ImageReference<2> ImageReference2D;
typedef ImageReference<3> ImageReference3D;

class Mesh;

/* AbstractQuery is not used directly */
class PrimitiveQuery;
class SampleQuery;
class TimeQuery;

class Renderbuffer;
#ifndef CORRADE_GCC45_COMPATIBILITY
enum class RenderbufferFormat: GLenum;
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class ResourceState: UnsignedByte;
enum class ResourceDataState: UnsignedByte;
enum class ResourcePolicy: UnsignedByte;
#endif
template<class T, class U = T> class Resource;
class ResourceKey;
template<class...> class ResourceManager;

class Sampler;
class Shader;

template<UnsignedInt> class Texture;
#ifndef MAGNUM_TARGET_GLES
typedef Texture<1> Texture1D;
#endif
typedef Texture<2> Texture2D;
typedef Texture<3> Texture3D;

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class TextureFormat: GLenum;
#endif

class Timeline;
#endif

}

#endif
