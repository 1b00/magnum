#ifndef Magnum_Extensions_h
#define Magnum_Extensions_h
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
 * @brief Namespace Magnum::Extensions
 */

#include "Context.h"

namespace Magnum {

/**
@brief Compile-time information about OpenGL extensions

Each extension is `struct` named hierarchically by prefix, vendor and
extension name, for example `GL::APPLE::vertex_array_object`. Each struct has
the same public methods as Extension class (requiredVersion(), coreVersion()
and string(), but these structs are better suited for compile-time decisions
rather than Extension instances. See Context::isExtensionSupported() for
example usage.

@todo Manual indices for extensions, this has gaps
@todo Unhide ES2_compatibility, ES3_compatibility on ES
@todo Add ES and GL 4.3 extensions
*/
namespace Extensions {

#ifndef MAGNUM_TARGET_GLES
#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(prefix, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = __LINE__-1 };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };
namespace GL {
    #line 1
    namespace AMD {
        _extension(GL,AMD,shader_trinary_minmax,        GL210,  None) // #428
    } namespace APPLE {
        _extension(GL,APPLE,flush_buffer_range,         GL210, GL300) // #321
        _extension(GL,APPLE,vertex_array_object,        GL210, GL300) // #273
    } namespace ARB {
        _extension(GL,ARB,texture_rectangle,            GL210, GL310) // #38
        _extension(GL,ARB,color_buffer_float,           GL210, GL300) // #39
        _extension(GL,ARB,half_float_pixel,             GL210, GL300) // #40
        _extension(GL,ARB,texture_float,                GL210, GL300) // #41
        _extension(GL,ARB,depth_buffer_float,           GL210, GL300) // #43
        _extension(GL,ARB,draw_instanced,               GL210, GL310) // #44
        _extension(GL,ARB,geometry_shader4,             GL210, GL320) // #47
        _extension(GL,ARB,instanced_arrays,             GL210, GL330) // #49
        _extension(GL,ARB,texture_buffer_object,        GL210, GL310) // #51
        _extension(GL,ARB,texture_rg,                   GL210, GL300) // #53
        _extension(GL,ARB,uniform_buffer_object,        GL210, GL310) // #57
        _extension(GL,ARB,copy_buffer,            /*?*/ GL210, GL310) // #59
        _extension(GL,ARB,depth_clamp,            /*?*/ GL210, GL320) // #61
        _extension(GL,ARB,draw_elements_base_vertex, /*?*/ GL210, GL320) // #62
        _extension(GL,ARB,fragment_coord_conventions, /*?*/ GL210, GL320) // #63
        _extension(GL,ARB,provoking_vertex,       /*?*/ GL210, GL320) // #64
        _extension(GL,ARB,seamless_cube_map,            GL210, GL320) // #65
        _extension(GL,ARB,sync,                         GL310, GL320) // #66
        _extension(GL,ARB,texture_multisample,    /*?*/ GL210, GL320) // #67
        _extension(GL,ARB,vertex_array_bgra,            GL210, GL320) // #68
        _extension(GL,ARB,draw_buffers_blend,           GL210, GL400) // #69
        _extension(GL,ARB,sample_shading,               GL210, GL400) // #70
        _extension(GL,ARB,texture_cube_map_array, /*?*/ GL210, GL400) // #71
        _extension(GL,ARB,texture_gather,               GL210, GL400) // #72
        _extension(GL,ARB,texture_query_lod,            GL210, GL400) // #73
        _extension(GL,ARB,texture_compression_bptc,     GL310, GL420) // #77
        _extension(GL,ARB,blend_func_extended,          GL210, GL330) // #78
        _extension(GL,ARB,explicit_attrib_location,     GL210, GL330) // #79
        _extension(GL,ARB,occlusion_query2,             GL210, GL330) // #80
        _extension(GL,ARB,sampler_objects,              GL210, GL330) // #81
        _extension(GL,ARB,shader_bit_encoding,    /*?*/ GL210, GL330) // #82
        _extension(GL,ARB,texture_rgb10_a2ui,           GL210, GL330) // #83
        _extension(GL,ARB,texture_swizzle,        /*?*/ GL210, GL330) // #84
        _extension(GL,ARB,timer_query,            /*?*/ GL210, GL330) // #85
        _extension(GL,ARB,vertex_type_2_10_10_10_rev,   GL210, GL330) // #86
        _extension(GL,ARB,draw_indirect,                GL310, GL400) // #87
        _extension(GL,ARB,gpu_shader5,                  GL320, GL400) // #88
        _extension(GL,ARB,gpu_shader_fp64,              GL320, GL400) // #89
        _extension(GL,ARB,shader_subroutine,            GL320, GL400) // #90
        _extension(GL,ARB,tessellation_shader,          GL320, GL400) // #91
        _extension(GL,ARB,texture_buffer_object_rgb32, /*?*/ GL210, GL400) // #92
        _extension(GL,ARB,transform_feedback2,          GL210, GL400) // #93
        _extension(GL,ARB,transform_feedback3,          GL210, GL400) // #94
        _extension(GL,ARB,ES2_compatibility,      /*?*/ GL210, GL410) // #95
        _extension(GL,ARB,get_program_binary,           GL300, GL410) // #96
        _extension(GL,ARB,separate_shader_objects,      GL210, GL410) // #97
        _extension(GL,ARB,shader_precision,             GL400, GL410) // #98
        _extension(GL,ARB,vertex_attrib_64bit,          GL300, GL410) // #99
        _extension(GL,ARB,viewport_array,               GL210, GL410) // #100
        _extension(GL,ARB,base_instance,                GL210, GL420) // #107
        _extension(GL,ARB,shading_language_420pack,     GL300, GL420) // #108
        _extension(GL,ARB,transform_feedback_instanced, GL210, GL420) // #109
        _extension(GL,ARB,compressed_texture_pixel_storage, GL210, GL420) // #110
        _extension(GL,ARB,conservative_depth,           GL300, GL420) // #111
        _extension(GL,ARB,internalformat_query,         GL210, GL420) // #112
        _extension(GL,ARB,map_buffer_alignment,         GL210, GL420) // #113
        _extension(GL,ARB,shader_atomic_counters,       GL300, GL420) // #114
        _extension(GL,ARB,shader_image_load_store,      GL300, GL420) // #115
        _extension(GL,ARB,texture_storage,              GL210, GL420) // #117
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic,   GL210,  None) // #187
        _extension(GL,EXT,framebuffer_object,           GL210, GL300) // #310
        _extension(GL,EXT,packed_depth_stencil,         GL210, GL300) // #312
        _extension(GL,EXT,framebuffer_blit,             GL210, GL300) // #316
        _extension(GL,EXT,framebuffer_multisample,      GL210, GL300) // #317
        _extension(GL,EXT,gpu_shader4,                  GL210, GL300) // #326
        _extension(GL,EXT,packed_float,                 GL210, GL300) // #328
        _extension(GL,EXT,texture_array,                GL210, GL300) // #329
        _extension(GL,EXT,texture_compression_rgtc,     GL210, GL300) // #332
        _extension(GL,EXT,texture_shared_exponent,      GL210, GL300) // #333
        _extension(GL,EXT,framebuffer_sRGB,             GL210, GL300) // #337
        _extension(GL,EXT,draw_buffers2,                GL210, GL300) // #340
        _extension(GL,EXT,texture_integer,              GL210, GL300) // #343
        _extension(GL,EXT,transform_feedback,           GL210, GL300) // #352
        _extension(GL,EXT,direct_state_access,          GL210,  None) // #353
        _extension(GL,EXT,texture_snorm,                GL300, GL310) // #365
    } namespace INTEL {
        /* INTEL_map_texture not supported */                         // #429
    } namespace NV {
        _extension(GL,NV,half_float,                    GL210, GL300) // #283
        _extension(GL,NV,primitive_restart,             GL210, GL310) // #285
        _extension(GL,NV,depth_buffer_float,            GL210, GL300) // #334
        _extension(GL,NV,conditional_render,            GL210, GL300) // #346
        /* NV_draw_texture not supported */                           // #430
    }
}
#undef _extension
#endif
#endif

}

}

#endif
