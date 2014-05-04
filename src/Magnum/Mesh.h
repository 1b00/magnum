#ifndef Magnum_Mesh_h
#define Magnum_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Mesh
 */

#include <vector>
#include <Corrade/Utility/ConfigurationValue.h>

#include "Magnum/AbstractShaderProgram.h"

namespace Magnum {

/**
 * @brief %Mesh primitive type
 *
 * @see @ref Mesh::primitive(), @ref Mesh::setPrimitive()
 */
enum class MeshPrimitive: GLenum {
    /** Single points. */
    Points = GL_POINTS,

    /**
     * First two vertices define first line segment, each following
     * vertex defines another segment.
     */
    LineStrip = GL_LINE_STRIP,

    /** Line strip, last and first vertex are connected together. */
    LineLoop = GL_LINE_LOOP,

    /**
     * Each pair of vertices defines a single line, lines aren't
     * connected together.
     */
    Lines = GL_LINES,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Line strip with adjacency information.
     * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES.
     */
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,

    /**
     * Lines with adjacency information.
     * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES.
     */
    LinesAdjacency = GL_LINES_ADJACENCY,
    #endif

    /**
     * First three vertices define first triangle, each following
     * vertex defines another triangle.
     */
    TriangleStrip = GL_TRIANGLE_STRIP,

    /**
     * First vertex is center, each following vertex is connected to
     * previous and center vertex.
     */
    TriangleFan = GL_TRIANGLE_FAN,

    /** Each three vertices define one triangle. */
    Triangles = GL_TRIANGLES,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Triangle strip with adjacency information.
     * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES.
     */
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,

    /**
     * Triangles with adjacency information.
     * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES.
     */
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,

    /**
     * Patches.
     * @requires_gl40 %Extension @extension{ARB,tessellation_shader}
     * @requires_gl Tessellation shaders are not available in OpenGL ES.
     */
    Patches = GL_PATCHES
    #endif
};

namespace Implementation { struct MeshState; }

/**
@brief %Mesh

@section Mesh-configuration Mesh configuration

You have to specify at least primitive and vertex/index count using
@ref setPrimitive() and @ref setCount(). Then fill your vertex buffers with
data, add them to the mesh and specify
@ref AbstractShaderProgram::Attribute "shader attribute" layout inside the
buffers using @ref addVertexBuffer(). You can also use
@ref MeshTools::interleave() to conveniently interleave vertex data.

If you want indexed mesh, fill your index buffer with data and specify its
layout using @ref setIndexBuffer(). You can also use @ref MeshTools::compressIndices()
to conveniently compress the indices based on the range used.

There is also @ref MeshTools::compile() function which operates directly on
@ref Trade::MeshData2D / @ref Trade::MeshData3D and returns fully configured
mesh and vertex/index buffers for use with stock shaders.

Note that neither vertex buffers nor index buffer is managed (e.g. deleted on
destruction) by the mesh, so you have to manage them on your own and ensure
that they are available for whole mesh lifetime. On the other hand it allows
you to use one buffer for more meshes (each mesh for example configured for
different usage) or store data for more meshes in one buffer.

If vertex/index count or instance count is zero, the mesh is empty and no draw
commands are issued when calling @ref draw().

@subsection Mesh-configuration-examples Example mesh configuration

@subsubsection Mesh-configuration-examples-nonindexed Basic non-indexed mesh

@code
// Custom shader, needing only position data
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

// Fill vertex buffer with position data
static constexpr Vector3 positions[30] = {
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

// Configure the mesh, add vertex buffer
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(30)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{});
@endcode

@subsubsection Mesh-configuration-examples-nonindexed-phong Interleaved vertex data

@code
// Non-indexed primitive with positions and normals
Trade::MeshData3D plane = Primitives::Plane::solid();

// Fill vertex buffer with interleaved position and normal data
Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(plane.positions(0), plane.normals(0)), BufferUsage::StaticDraw);

// Configure the mesh, add vertex buffer
Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(plane.positions(0).size())
    .addVertexBuffer(buffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{});
@endcode

@subsubsection Mesh-configuration-examples-indexed-phong Indexed mesh

@code
// Custom shader
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

// Fill vertex buffer with position data
static constexpr Vector3 positions[300] = {
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

// Fill index buffer with index data
static constexpr GLubyte indices[75] = {
    // ...
};
Buffer indexBuffer;
indexBuffer.setData(indices, BufferUsage::StaticDraw);

// Configure the mesh, add both vertex and index buffer
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(75)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 176, 229);
@endcode

Or using @ref MeshTools::interleave() and @ref MeshTools::compressIndices():

@code
// Indexed primitive
Trade::MeshData3D cube = Primitives::Cube::solid();

// Fill vertex buffer with interleaved position and normal data
Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)), BufferUsage::StaticDraw);

// Compress index data
Containers::Array<char> indexData;
Mesh::IndexType indexType;
UnsignedInt indexStart, indexEnd;
std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());

// Fill index buffer
Buffer indexBuffer;
indexBuffer.setData(data);

// Configure the mesh, add both vertex and index buffer
Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(cube.indices().size())
    .addVertexBuffer(vertexBuffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
    .setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);
@endcode

Or, if you plan to use the mesh with stock shaders, you can just use
@ref MeshTools::compile().

@subsubsection Mesh-configuration-examples-data-options Specific formats of vertex data

@code
// Custom shader with colors specified as four floating-point values
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;
        typedef Attribute<1, Color4> Color;

    // ...
};

// Initial mesh configuration
Mesh mesh;
mesh.setPrimitive(...)
    .setCount(30);

// Fill position buffer with positions specified as two-component XY (i.e.,
// no Z component, which is meant to be always 0)
Vector2 positions[30] = {
    // ...
};
Buffer positionBuffer;
positionBuffer.setData(positions, BufferUsage::StaticDraw);

// Specify layout of positions buffer -- only two components, unspecified Z
// component will be automatically set to 0
mesh.addVertexBuffer(positionBuffer, 0,
    MyShader::Position{MyShader::Position::Components::Two});

// Fill color buffer with colors specified as four-byte BGRA (e.g. directly
// from TGA file)
GLubyte colors[4*30] = {
    // ...
};
Buffer colorBuffer;
colorBuffer.setData(colors, BufferUsage::StaticDraw);

// Specify layout of color buffer -- BGRA, each component unsigned byte and we
// want to normalize them from [0, 255] to [0.0f, 1.0f]
mesh.addVertexBuffer(colorBuffer, 0, MyShader::Color{
    MyShader::Color::Components::BGRA,
    MyShader::Color::DataType::UnsignedByte,
    MyShader::Color::DataOption::Normalized});
@endcode

@section Mesh-drawing Rendering meshes

Basic workflow is: bind specific framebuffer for drawing (if needed), set up
respective shader (see
@ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
for more infromation) and call @ref Mesh::draw().

@section Mesh-webgl-restrictions WebGL restrictions

@ref MAGNUM_TARGET_WEBGL "WebGL" puts some restrictions on vertex buffer
layout, see @ref addVertexBuffer() documentation for details.

@section Mesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object} (part of OpenGL 3.0), OpenGL ES 3.0 or
@es_extension{OES,vertex_array_object} on OpenGL ES 2.0 is supported, VAOs are
used instead of binding the buffers and specifying vertex attribute pointers
in each @ref draw() call. The engine tracks currently bound VAO and currently
active shader program to avoid unnecessary calls to @fn_gl{BindVertexArray} and
@fn_gl{UseProgram}. %Mesh limits and implementation-defined values (such as
@ref maxVertexAttributes()) are cached, so repeated queries don't result in
repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} and VAOs are available,
DSA functions are used for specifying attribute locations to avoid unnecessary
calls to @fn_gl{BindBuffer} and @fn_gl{BindVertexArray}. See documentation of
@ref addVertexBuffer() for more information.

If index range is specified in @ref setIndexBuffer(), range-based version of
drawing commands are used on desktop OpenGL and OpenGL ES 3.0. See also
@ref draw() for more information.
 */
class MAGNUM_EXPORT Mesh: public AbstractObject {
    friend class MeshView;
    friend struct Implementation::MeshState;

    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief MeshPrimitive
         * @deprecated Use @ref Magnum::MeshPrimitive "MeshPrimitive" instead.
         */
        typedef CORRADE_DEPRECATED("use MeshPrimitive instead") MeshPrimitive Primitive;
        #endif

        /**
         * @brief Index type
         *
         * @see @ref setIndexBuffer(), @ref indexSize()
         */
        enum class IndexType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */

            /**
             * Unsigned int
             * @requires_gles30 %Extension @es_extension{OES,element_index_uint}
             *      in OpenGL ES 2.0
             */
            UnsignedInt = GL_UNSIGNED_INT
        };

        /**
         * @brief Max supported vertex attribute count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. This function is in fact alias to
         * @ref AbstractShaderProgram::maxVertexAttributes().
         * @see @ref addVertexBuffer()
         */
        static Int maxVertexAttributes();

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max recommended index count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setIndexBuffer(), @fn_gl{Get} with @def_gl{MAX_ELEMENTS_INDICES}
         * @requires_gles30 Ranged element draw is not supported in OpenGL ES
         *      2.0.
         */
        static Int maxElementsIndices();

        /**
         * @brief Max recommended vertex count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setIndexBuffer(), @fn_gl{Get} with @def_gl{MAX_ELEMENTS_VERTICES}
         * @requires_gles30 Ranged element draw is not supported in OpenGL ES
         *      2.0.
         */
        static Int maxElementsVertices();
        #endif

        /**
         * @brief Size of given index type
         *
         * @see @ref indexSize() const
         */
        static std::size_t indexSize(IndexType type);

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates mesh with no vertex buffers and zero vertex count.
         * @see @ref setPrimitive(), @ref setCount(), @fn_gl{GenVertexArrays}
         *      (if @extension{APPLE,vertex_array_object} is available)
         */
        explicit Mesh(MeshPrimitive primitive = MeshPrimitive::Triangles);

        /** @brief Copying is not allowed */
        Mesh(const Mesh&) = delete;

        /** @brief Move constructor */
        Mesh(Mesh&& other) noexcept;

        /**
         * @brief Destructor
         *
         * @see @fn_gl{DeleteVertexArrays} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        ~Mesh();

        /** @brief Copying is not allowed */
        Mesh& operator=(const Mesh&) = delete;

        /** @brief Move assignment */
        Mesh& operator=(Mesh&& other) noexcept;

        /**
         * @brief OpenGL mesh ID
         *
         * If @extension{APPLE,vertex_array_object} (part of OpenGL 3.0) is not
         * available, returns `0`.
         */
        GLuint id() const { return _id; }

        /**
         * @brief %Mesh label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{VERTEX_ARRAY} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{VERTEX_ARRAY_OBJECT_EXT}
         */
        std::string label() const;

        /**
         * @brief Set mesh label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{VERTEX_ARRAY} or @fn_gl_extension2{LabelObject,EXT,debug_label}
         *      with @def_gl{VERTEX_ARRAY_OBJECT_EXT}
         */
        Mesh& setLabel(const std::string& label);

        /**
         * @brief Whether the mesh is indexed
         *
         * @see @ref setIndexBuffer(), @ref setCount()
         */
        bool isIndexed() const { return _indexBuffer; }

        /**
         * @brief Index size
         *
         * @see @ref indexSize(IndexType)
         */
        std::size_t indexSize() const { return indexSize(_indexType); }

        /** @brief Primitive type */
        MeshPrimitive primitive() const { return _primitive; }

        /**
         * @brief Set primitive type
         * @return Reference to self (for method chaining)
         *
         * Default is @ref MeshPrimitive::Triangles.
         */
        Mesh& setPrimitive(MeshPrimitive primitive) {
            _primitive = primitive;
            return *this;
        }

        /** @brief Vertex/index count */
        Int count() const { return _count; }

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * If the mesh is indexed, the value is treated as index count,
         * otherwise the value is vertex count. If set to `0`, no draw commands
         * are issued when calling @ref draw(). Default is `0`.
         * @see @ref isIndexed(), @ref setBaseVertex(), @ref setInstanceCount()
         */
        Mesh& setCount(Int count) {
            _count = count;
            return *this;
        }

        /** @brief Base vertex */
        Int baseVertex() const { return _baseVertex; }

        /**
         * @brief Set base vertex
         * @return Reference to self (for method chaining)
         *
         * Sets number of vertices of which the vertex buffer will be offset
         * when drawing. Default is `0`.
         * @see @ref setCount(), @ref setBaseInstance()
         * @requires_gl32 %Extension @extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gl Base vertex cannot be specified for indexed meshes in
         *      OpenGL ES.
         */
        Mesh& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief count()
         * @deprecated Use @ref Magnum::Mesh::count() "count()" instead.
         */
        CORRADE_DEPRECATED("use count() instead") Int vertexCount() const {
            return isIndexed() ? 0 : count();
        }

        /**
         * @copybrief setCount()
         * @deprecated Use @ref Magnum::Mesh::setCount() "setCount()" instead.
         */
        CORRADE_DEPRECATED("use setCount() instead") Mesh& setVertexCount(Int count) {
            if(!isIndexed()) setCount(count);
            return *this;
        }

        /**
         * @copybrief count()
         * @deprecated Use @ref Magnum::Mesh::count() "count()" instead.
         */
        CORRADE_DEPRECATED("use count() instead") Int indexCount() const {
            return count();
        }

        /**
         * @copybrief setCount()
         * @deprecated Use @ref Magnum::Mesh::setCount() "setCount()" instead.
         */
        CORRADE_DEPRECATED("use setCount() instead") Mesh& setIndexCount(Int count) { return setCount(count); }
        #endif

        /** @brief Instance count */
        Int instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * If set to `1`, non-instanced draw commands are issued when calling
         * @ref draw(). If set to `0`, no draw commands are issued altogether.
         * Default is `1`.
         * @see @ref setBaseInstance(), @ref setCount(),
         *      @ref addVertexBufferInstanced()
         * @requires_gl31 %Extension @extension{ARB,draw_instanced}
         * @requires_gles30 %Extension @es_extension{ANGLE,instanced_arrays},
         *      @es_extension2{EXT,draw_instanced,draw_instanced} or
         *      @es_extension{NV,draw_instanced} in OpenGL ES 2.0.
         */
        Mesh& setInstanceCount(Int count) {
            _instanceCount = count;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @brief Base instance */
        UnsignedInt baseInstance() const { return _baseInstance; }

        /**
         * @brief Set base instance
         * @return Reference to self (for method chaining)
         *
         * Default is `0`.
         * @see @ref setInstanceCount(), @ref setBaseVertex()
         * @requires_gl42 %Extension @extension{ARB,base_instance}
         * @requires_gl Base instance cannot be specified in OpenGL ES.
         */
        Mesh& setBaseInstance(UnsignedInt baseInstance) {
            _baseInstance = baseInstance;
            return *this;
        }
        #endif

        /**
         * @brief Add buffer with (interleaved) vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Parameter @p offset is offset of the array from the beginning,
         * attribute list is combination of @ref AbstractShaderProgram::Attribute "attribute definitions"
         * (specified in implementation of given shader) and offsets between
         * interleaved attributes.
         *
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 76 bytes of some other data at the beginning (possibly material
         * configuration) and then the interleaved vertex array. Each vertex
         * consists of weight, position, texture coordinate and normal. You
         * want to draw it with @ref Shaders::Phong, but it accepts only
         * position and normal, so you have to skip weight and texture
         * coordinate in each vertex:
         * @code
         * Buffer buffer;
         * Mesh mesh;
         * mesh.addVertexBuffer(buffer, 76, // initial array offset
         *     4,                           // skip vertex weight (Float)
         *     Shaders::Phong::Position(),  // vertex position
         *     8,                           // skip texture coordinates (Vector2)
         *     Shaders::Phong::Normal());   // vertex normal
         * @endcode
         *
         * You can also achieve the same effect by calling @ref addVertexBuffer()
         * more times with explicitly specified gaps before and after the
         * attributes. This can be used for e.g. runtime-dependent
         * configuration, as it isn't dependent on the variadic template:
         * @code
         * mesh.addVertexBuffer(buffer, 76, 4, Shaders::Phong::Position(), 20)
         *     .addVertexBuffer(buffer, 76, 24, Shaders::Phong::Normal(), 0);
         * @endcode
         *
         * If specifying more than one attribute, the function assumes that
         * the array is interleaved. Adding non-interleaved vertex buffer can
         * be done by specifying one attribute at a time with specific offset.
         * Above example with weight, position, texture coordinate and normal
         * arrays one after another (non-interleaved):
         * @code
         * Int vertexCount = 352;
         * mesh.addVertexBuffer(buffer, 76 + 4*vertexCount, Shaders::Phong::Position())
         *     .addVertexBuffer(buffer, 76 + 24*vertexCount, Shaders::Phong::Normal());
         * @endcode
         *
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @attention In @ref MAGNUM_TARGET_WEBGL "WebGL" the data must be
         *      properly aligned (e.g. all float data must start at addresses
         *      divisible by four). Also the maximum stride of attribute data
         *      must be at most 255 bytes. This is not required anywhere else,
         *      but doing so may have performance benefits.
         *
         * @see @ref addVertexBufferInstanced(), @ref maxVertexAttributes(),
         *      @ref setPrimitive(), @ref setCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         */
        template<class ...T> inline Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), 0, attributes...);
            return *this;
        }

        /**
         * @brief Add instanced vertex buffer
         * @return Reference to self (for method chaining)
         *
         * Similar to the above function, the @p divisor parameter specifies
         * number of instances that will pass until new data are fetched from
         * the buffer. Setting it to `0` is equivalent to calling
         * @ref addVertexBuffer().
         * @see @ref maxVertexAttributes(), @ref setPrimitive(),
         *      @ref setCount(), @ref setInstanceCount(), @ref setBaseInstance(),
         *      @fn_gl{BindVertexArray}, @fn_gl{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl{VertexAttribPointer},
         *      @fn_gl{VertexAttribDivisor} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribDivisor,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         * @requires_gl33 %Extension @extension{ARB,instanced_arrays}
         * @requires_gles30 %Extension @es_extension{ANGLE,instanced_arrays},
         *      @es_extension{EXT,instanced_arrays} or
         *      @es_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         */
        template<class ...T> inline Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), divisor, attributes...);
            return *this;
        }

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @param start         Minimum array index contained in the buffer
         * @param end           Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         *
         * The smaller range is specified with @p start and @p end the less
         * memory operations are needed (and possibly some optimizations),
         * improving draw performance. Specifying `0` for both parameters
         * behaves the same as @ref setIndexBuffer(Buffer&, GLintptr, IndexType).
         * On OpenGL ES 2.0 this function behaves always as
         * @ref setIndexBuffer(Buffer&, GLintptr, IndexType), as this
         * functionality is not available there.
         * @see @ref maxElementsIndices(), @ref maxElementsVertices(),
         *      @ref setCount(), @ref isIndexed(), @fn_gl{BindVertexArray},
         *      @fn_gl{BindBuffer} (if @extension{APPLE,vertex_array_object} is
         *      available)
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @return Reference to self (for method chaining)
         *
         * Alternative to @ref setIndexBuffer(Buffer&, GLintptr, IndexType, UnsignedInt, UnsignedInt)
         * with unspecified index limits, see its documentation for more
         * information. Prefer to set index limits for better performance.
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type) {
            return setIndexBuffer(buffer, offset, type, 0, 0);
        }

        /**
         * @brief Draw the mesh
         * @param shader    Shader to use for drawing
         *
         * Expects that the shader is compatible with this mesh and is fully
         * set up. If vertex/index count or instance count is `0`, no draw
         * commands are issued. See also
         * @ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
         * for more information.
         * @see @ref setCount(), @ref setInstanceCount(), @fn_gl{UseProgram},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer}, @fn_gl{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray} (if @extension{APPLE,vertex_array_object}
         *      is available), @fn_gl{DrawArrays}/@fn_gl{DrawArraysInstanced}/
         *      @fn_gl{DrawArraysInstancedBaseInstance} or @fn_gl{DrawElements}/
         *      @fn_gl{DrawRangeElements}/@fn_gl{DrawElementsBaseVertex}/
         *      @fn_gl{DrawRangeElementsBaseVertex}/@fn_gl{DrawElementsInstanced}/
         *      @fn_gl{DrawElementsInstancedBaseInstance}/
         *      @fn_gl{DrawElementsInstancedBaseVertex}/
         *      @fn_gl{DrawElementsInstancedBaseVertexBaseInstance}
         */
        void draw(AbstractShaderProgram& shader) {
            shader.use();

            #ifndef MAGNUM_TARGET_GLES
            drawInternal(_count, _baseVertex, _instanceCount, _baseInstance, _indexOffset, _indexStart, _indexEnd);
            #elif !defined(MAGNUM_TARGET_GLES2)
            drawInternal(_count, _baseVertex, _instanceCount, _indexOffset, _indexStart, _indexEnd);
            #else
            drawInternal(_count, _baseVertex, _instanceCount, _indexOffset);
            #endif
        }

        void draw(AbstractShaderProgram&& shader) { draw(shader); } /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief draw(AbstractShaderProgram&)
         * @deprecated Use @ref Magnum::Mesh::draw(AbstractShaderProgram&) "draw(AbstractShaderProgram&)"
         *      instead.
         */
        CORRADE_DEPRECATED("use draw(AbstractShaderProgram&) instead") void draw() {
            #ifndef MAGNUM_TARGET_GLES
            drawInternal(_count, _baseVertex, _instanceCount, _baseInstance, _indexOffset, _indexStart, _indexEnd);
            #elif !defined(MAGNUM_TARGET_GLES2)
            drawInternal(_count, _baseVertex, _instanceCount, _indexOffset, _indexStart, _indexEnd);
            #else
            drawInternal(_count, _baseVertex, _instanceCount, _indexOffset);
            #endif
        }
        #endif

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct MAGNUM_LOCAL Attribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            bool normalized;
            GLintptr offset;
            GLsizei stride;
            GLuint divisor;
        };

        #ifndef MAGNUM_TARGET_GLES2
        struct MAGNUM_LOCAL IntegerAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
            GLuint divisor;
        };

        #ifndef MAGNUM_TARGET_GLES
        struct MAGNUM_LOCAL LongAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
            GLuint divisor;
        };
        #endif
        #endif
        #endif

        /* Computing stride of interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> static GLsizei strideOfInterleaved(const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            return attribute.vectorSize()*AbstractShaderProgram::Attribute<location, T>::VectorCount + strideOfInterleaved(attributes...);
        }
        template<class ...T> static GLsizei strideOfInterleaved(GLintptr gap, const T&... attributes) {
            return gap + strideOfInterleaved(attributes...);
        }
        static GLsizei strideOfInterleaved() { return 0; }

        /* Adding interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, stride, divisor);

            /* Add size of this attribute to offset for next attribute */
            addVertexBufferInternal(buffer, offset+attribute.vectorSize()*AbstractShaderProgram::Attribute<location, T>::VectorCount, stride, divisor, attributes...);
        }
        template<class ...T> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addVertexBufferInternal(buffer, offset+gap, stride, divisor, attributes...);
        }
        void addVertexBufferInternal(Buffer&, GLsizei, GLuint, GLintptr) {}

        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Float>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != AbstractShaderProgram::Attribute<location, T>::VectorCount; ++i)
                attributePointerInternal(Attribute{
                    &buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    bool(attribute.dataOptions() & AbstractShaderProgram::Attribute<location, T>::DataOption::Normalized),
                    GLintptr(offset+i*attribute.vectorSize()),
                    stride,
                    divisor
                });
        }

        #ifndef MAGNUM_TARGET_GLES2
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::ScalarType>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            attributePointerInternal(IntegerAttribute{
                &buffer,
                location,
                GLint(attribute.components()),
                GLenum(attribute.dataType()),
                offset,
                stride,
                divisor
            });
        }

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Double>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != AbstractShaderProgram::Attribute<location, T>::VectorCount; ++i)
                attributePointerInternal(LongAttribute{
                    &buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    GLintptr(offset+i*attribute.vectorSize()),
                    stride,
                    divisor
                });
        }
        #endif
        #endif

        static void MAGNUM_LOCAL bindVAO(GLuint vao);

        #ifndef MAGNUM_TARGET_GLES
        void drawInternal(Int count, Int baseVertex, Int instanceCount, UnsignedInt baseInstance, GLintptr indexOffset, Int indexStart, Int indexEnd);
        #elif !defined(MAGNUM_TARGET_GLES2)
        void drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset, Int indexStart, Int indexEnd);
        #else
        void drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset);
        #endif

        void MAGNUM_LOCAL createImplementationDefault();
        void MAGNUM_LOCAL createImplementationVAO();

        void MAGNUM_LOCAL destroyImplementationDefault();
        void MAGNUM_LOCAL destroyImplementationVAO();

        void attributePointerInternal(const Attribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const Attribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const Attribute& attribute);
        #endif
        void MAGNUM_LOCAL vertexAttribPointer(const Attribute& attribute);

        #ifndef MAGNUM_TARGET_GLES2
        void attributePointerInternal(const IntegerAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const IntegerAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const IntegerAttribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const IntegerAttribute& attribute);
        #endif
        void MAGNUM_LOCAL vertexAttribPointer(const IntegerAttribute& attribute);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void attributePointerInternal(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDSA(const LongAttribute& attribute);
        void MAGNUM_LOCAL vertexAttribPointer(const LongAttribute& attribute);
        #endif

        #ifdef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL vertexAttribDivisorImplementationANGLE(GLuint index, GLuint divisor);
        void MAGNUM_LOCAL vertexAttribDivisorImplementationEXT(GLuint index, GLuint divisor);
        void MAGNUM_LOCAL vertexAttribDivisorImplementationNV(GLuint index, GLuint divisor);
        #endif

        void MAGNUM_LOCAL bindIndexBufferImplementationDefault(Buffer&);
        void MAGNUM_LOCAL bindIndexBufferImplementationVAO(Buffer& buffer);

        void MAGNUM_LOCAL bindImplementationDefault();
        void MAGNUM_LOCAL bindImplementationVAO();

        void MAGNUM_LOCAL unbindImplementationDefault();
        void MAGNUM_LOCAL unbindImplementationVAO();

        #ifdef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL drawArraysInstancedImplementationANGLE(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        void MAGNUM_LOCAL drawArraysInstancedImplementationEXT(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        void MAGNUM_LOCAL drawArraysInstancedImplementationNV(GLint baseVertex, GLsizei count, GLsizei instanceCount);

        void MAGNUM_LOCAL drawElementsInstancedImplementationANGLE(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        void MAGNUM_LOCAL drawElementsInstancedImplementationEXT(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        void MAGNUM_LOCAL drawElementsInstancedImplementationNV(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        #endif

        GLuint _id;
        MeshPrimitive _primitive;
        Int _count, _baseVertex, _instanceCount;
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
        GLintptr _indexOffset;
        IndexType _indexType;
        Buffer* _indexBuffer;

        std::vector<Attribute> _attributes;
        #ifndef MAGNUM_TARGET_GLES2
        std::vector<IntegerAttribute> _integerAttributes;
        #ifndef MAGNUM_TARGET_GLES
        std::vector<LongAttribute> _longAttributes;
        #endif
        #endif
};

/** @debugoperator{Magnum::Mesh} */
Debug MAGNUM_EXPORT operator<<(Debug debug, MeshPrimitive value);

/** @debugoperator{Magnum::Mesh} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Mesh::IndexType value);

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Mesh} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::MeshPrimitive> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::MeshPrimitive value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns @ref Magnum::MeshPrimitive::Points "MeshPrimitive::Points".
     */
    static Magnum::MeshPrimitive fromString(const std::string& stringValue, ConfigurationValueFlags);
};

/** @configurationvalue{Magnum::Mesh} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::Mesh::IndexType> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::Mesh::IndexType value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns @ref Magnum::Mesh::IndexType::UnsignedInt "Mesh::IndexType::UnsignedInt".
     */
    static Magnum::Mesh::IndexType fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
