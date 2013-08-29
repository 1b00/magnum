#ifndef Magnum_Mesh_h
#define Magnum_Mesh_h
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
 * @brief Class Magnum::Mesh
 */

#include <vector>
#include <Utility/Assert.h>
#include <Utility/ConfigurationValue.h>

#include "AbstractShaderProgram.h"

namespace Magnum {

/**
@brief %Mesh

@section Mesh-configuration Mesh configuration

You have to specify at least primitive and vertex count using setPrimitive()
and setVertexCount(). Then fill your vertex buffers with data, add them to the
mesh and specify @ref AbstractShaderProgram::Attribute "shader attribute" layout
inside the buffers using addVertexBuffer(), addInterleavedVertexBuffer() or
addVertexBufferStride(). You can also use MeshTools::interleave() in
combination with addInterleavedVertexBuffer() to conveniently fill interleaved
vertex buffer(s). The function itself calls setVertexCount(), so you don't
have to do it again.

If you have indexed mesh, you need to call setIndexCount() instead of
setVertexCount(). Then fill your index buffer with data and specify its layout
using setIndexBuffer(). You can also use MeshTools::compressIndices() to
conveniently compress the indices, fill the index buffer and configure the
mesh instead of calling setIndexCount() and setIndexBuffer() manually.

Note that neither vertex buffers nor index buffer is managed (e.g. deleted on
destruction) by the mesh, so you have to manage them on your own and ensure
that they are available for whole mesh lifetime. On the other hand it allows
you to use one buffer for more meshes (each mesh for example configured for
different shader) or store data for more meshes in one buffer.

If the mesh has non-zero index count, it is treated as indexed mesh, otherwise
it is treated as non-indexed mesh. If both index and vertex count is zero, the
mesh is empty and no draw commands are issued when calling draw().

@subsection Mesh-configuration-examples Example mesh configuration

@subsubsection Mesh-configuration-examples-nonindexed Basic non-indexed mesh

@code
// Custom shader, needing only position data
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};
Buffer vertexBuffer;
Mesh mesh;

// Fill vertex buffer with position data
static constexpr Vector3 positions[30] = {
    // ...
};
vertexBuffer.setData(positions, Buffer::Usage::StaticDraw);

// Set primitive and vertex count, add the buffer and specify its layout
mesh.setPrimitive(Mesh::Primitive::Triangles)
    .setVertexCount(30)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position());
@endcode

@subsubsection Mesh-configuration-examples-nonindexed-phong Interleaved vertex data

@code
// Non-indexed primitive with positions and normals
Trade::MeshData3D plane = Primitives::Plane::solid();
Buffer vertexBuffer;
Mesh mesh;

// Fill vertex buffer with interleaved position and normal data
MeshTools::interleave(mesh, buffer, Buffer::Usage::StaticDraw,
    plane.positions(0), plane.normals(0));

// Set primitive and specify layout of interleaved vertex buffer, vertex count
// has been already set by MeshTools::interleave()
mesh.setPrimitive(plane.primitive())
    .addInterleavedVertexBuffer(buffer, 0,
        Shaders::PhongShader::Position(),
        Shaders::PhongShader::Normal());
@endcode

@subsubsection Mesh-configuration-examples-indexed-phong Indexed mesh

@code
// Custom shader
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};
Buffer vertexBuffer, indexBuffer;
Mesh mesh;

// Fill vertex buffer with position data
static constexpr Vector3 positions[300] = {
    // ...
};
vertexBuffer.setData(positions, Buffer::Usage::StaticDraw);

// Fill index buffer with index data
static constexpr GLubyte indices[75] = {
    // ...
};
indexBuffer.setData(indices, Buffer::Usage::StaticDraw);

// Set primitive, index count, specify the buffers
mesh.setPrimitive(Mesh::Primitive::Triangles)
    .setIndexCount(75)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position())
    .setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 176, 229);
@endcode

@code
// Indexed primitive
Trade::MeshData3D cube = Primitives::Cube::solid();
Buffer vertexBuffer, indexBuffer;
Mesh mesh;

// Fill vertex buffer with interleaved position and normal data
MeshTools::interleave(mesh, vertexBuffer, Buffer::Usage::StaticDraw,
    cube.positions(0), cube.normals(0));

// Fill index buffer with compressed index data
MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw,
    cube.indices());

// Set primitive and specify layout of interleaved vertex buffer. Index count
// and index buffer has been already specified by MeshTools::compressIndices().
mesh.setPrimitive(plane.primitive())
    .addInterleavedVertexBuffer(vertexBuffer, 0,
        Shaders::PhongShader::Position(),
        Shaders::PhongShader::Normal());
@endcode

@subsubsection Mesh-configuration-examples-data-options Specific formats of vertex data

@code
// Custom shader with colors specified as four floating-point values
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;
        typedef Attribute<1, Color4> Color;

    // ...
};
Mesh mesh;

// Fill position buffer with positions specified as two-component XY (i.e.,
// no Z component, which is meant to be always 0)
Buffer positionBuffer;
Vector2 positions[30] = {
    // ...
};

// Specify layout of positions buffer -- only two components, unspecified Z
// component will be automatically set to 0
mesh.addVertexBuffer(positionBuffer, 0,
    MyShader::Position(MyShader::Position::Components::Two));

// Fill color buffer with colors specified as four-byte BGRA (e.g. directly
// from TGA file)
Buffer colorBuffer;
GLubyte colors[4*30] = {
    // ...
};
colorBuffer.setData(colors, Buffer::Usage::StaticDraw);

// Specify layout of color buffer -- BGRA, each component unsigned byte and we
// want to normalize them from [0, 255] to [0.0f, 1.0f]
mesh.addVertexBuffer(colorBuffer, 0, MyShader::Color(
    MyShader::Color::Components::BGRA,
    MyShader::Color::DataType::UnsignedByte,
    MyShader::Color::DataOption::Normalized));
@endcode

@section Mesh-drawing Rendering meshes

Basic workflow is: bind specific framebuffer for drawing (if needed), set up
respective shader, bind required textures (see
@ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
for more infromation) and call Mesh::draw().

@section Mesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object}, OpenGL ES 3.0 or
@es_extension{OES,vertex_array_object} on OpenGL ES 2.0 is supported, VAOs are
used instead of binding the buffers and specifying vertex attribute pointers
in each draw() call. The engine tracks currently bound VAO to avoid
unnecessary calls to @fn_gl{BindVertexArray}.

If extension @extension{EXT,direct_state_access} and VAOs are available,
DSA functions are used for specifying attribute locations to avoid unnecessary
calls to @fn_gl{BindBuffer} and @fn_gl{BindVertexArray}. See documentation of
addVertexBuffer(), addInterleavedVertexBuffer(), addVertexBufferStride() for
more information.

If index range is specified in setIndexBuffer(), range-based version of
drawing commands are used on desktop OpenGL and OpenGL ES 3.0. See also draw()
for more information.

@todo Support for indirect draw buffer (OpenGL 4.0, @extension{ARB,draw_indirect})
@todo Redo in a way that allows glMultiDrawArrays, glDrawArraysInstanced etc.
@todo test vertex specification & drawing
@todo How to glDrawElementsBaseVertex()/vertex offset -- in draw()?
 */
class MAGNUM_EXPORT Mesh {
    friend class Context;
    friend class MeshView;

    public:
        /**
         * @brief Primitive type
         *
         * @see primitive(), setPrimitive()
         */
        enum class Primitive: GLenum {
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
             */
            LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,

            /**
             * Lines with adjacency information.
             * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
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
             */
            TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,

            /**
             * Triangles with adjacency information.
             * @requires_gl32 %Extension @extension{ARB,geometry_shader4}
             */
            TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,

            /**
             * Patches.
             * @requires_gl40 %Extension @extension{ARB,tessellation_shader}
             */
            Patches = GL_PATCHES
            #endif
        };

        /**
         * @brief Index type
         *
         * @see setIndexBuffer(), indexSize()
         */
        enum class IndexType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */

            /**
             * Unsigned int
             * @requires_gles30 %Extension @es_extension{OES,element_index_uint}
             */
            UnsignedInt = GL_UNSIGNED_INT
        };

        /**
         * @brief Size of given index type
         *
         * @see indexSize() const
         */
        static std::size_t indexSize(IndexType type);

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates mesh with no vertex buffers and zero vertex count.
         * @see setPrimitive(), setVertexCount(), @fn_gl{GenVertexArrays} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        explicit Mesh(Primitive primitive = Primitive::Triangles);

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
         * @brief Index size
         *
         * @see indexSize(IndexType)
         */
        std::size_t indexSize() const { return indexSize(_indexType); }

        /** @brief Primitive type */
        Primitive primitive() const { return _primitive; }

        /**
         * @brief Set primitive type
         * @return Reference to self (for method chaining)
         *
         * Default is @ref Primitive::Triangles.
         * @see @ref setVertexCount(), @ref addVertexBuffer(),
         *      @ref addInterleavedVertexBuffer(), @ref addVertexBufferStride()
         */
        Mesh& setPrimitive(Primitive primitive) {
            _primitive = primitive;
            return *this;
        }

        /** @brief Vertex count */
        Int vertexCount() const { return _vertexCount; }

        /**
         * @brief Set vertex count
         * @return Reference to self (for method chaining)
         *
         * Default is zero.
         * @see setPrimitive(), addVertexBuffer(), addInterleavedVertexBuffer(),
         *      addVertexBufferStride(), MeshTools::interleave()
         */
        Mesh& setVertexCount(Int vertexCount) {
            _vertexCount = vertexCount;
            return *this;
        }

        /** @brief Index count */
        Int indexCount() const { return _indexCount; }

        /**
         * @brief Set index count
         * @return Reference to self (for method chaining)
         *
         * Default is zero.
         * @see setIndexBuffer(), MeshTools::compressIndices()
         */
        Mesh& setIndexCount(Int count) {
            _indexCount = count;
            return *this;
        }

        /**
         * @brief Add buffer with non-interleaved vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Attribute list is combination of
         * @ref AbstractShaderProgram::Attribute "attribute definitions"
         * (specified in implementation of given shader) and gaps between
         * attribute arrays.
         *
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 35 bytes of some other data at the beginning (possibly material
         * configuration), then position array, then texture coordinate array
         * and then normal array. You want to draw it with Shaders::PhongShader,
         * but it accepts only position and normal, so you have to skip the
         * texture coordinate array:
         * @code
         * Buffer buffer;
         * Mesh mesh;
         * mesh.addVertexBuffer(buffer,
         *     35,                                  // offset of the data
         *     Shaders::PhongShader::Position(),    // position array
         *     sizeof(Vector2)*mesh.vertexCount(),  // skip texture coordinate array
         *     Shaders::PhongShader::Normal());     // normal array
         * @endcode
         *
         * Vou can also achieve the same effect by calling this function more
         * times with absolute offsets:
         * @code
         * mesh.addVertexBuffer(buffer, 35, Shaders::PhongShader::Position());
         *     .addVertexBuffer(buffer, 35 + (sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2))*
         *          mesh->vertexCount(), Shaders::PhongShader::Normal());
         * @endcode
         *
         * @attention If specifying more than one attribute the actual vertex
         *      count must be set before calling this function. Otherwise
         *      vertex data positions in the buffer will be miscalculated.
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addInterleavedVertexBuffer(), addVertexBufferStride(),
         *      setPrimitive(), setVertexCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         */
        template<class ...T> Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes);

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Parameter @p offset is offset of the interleaved array from the
         * beginning, attribute list is combination of
         * @ref AbstractShaderProgram::Attribute "attribute definitions"
         * (specified in implementation of given shader) and offsets between
         * attributes.
         *
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 35 bytes of some other data at the beginning (possibly material
         * configuration) and then the interleaved vertex array. Each vertex
         * consists of weight, position, texture coordinate and normal. You
         * want to draw it with Shaders::PhongShader, but it accepts only
         * position and normal, so you have to skip weight and texture
         * coordinate in each vertex:
         * @code
         * Buffer buffer;
         * Mesh mesh;
         * mesh.addInterleavedVertexBuffer(buffer,
         *     35,                                  // skip other data
         *     sizeof(Float),                       // skip vertex weight
         *     Shaders::PhongShader::Position(),    // vertex position
         *     sizeof(Vector2),                     // skip texture coordinates
         *     Shaders::PhongShader::Normal());     // vertex normal
         * @endcode
         *
         * You can also achieve the same effect by calling addVertexBufferStride()
         * more times with absolute offset from the beginning and stride
         * between vertex attributes:
         * @code
         * GLsizei stride =                         // size of one vertex
         *     sizeof(Float) +
         *     sizeof(Shaders::PhongShader::Position::Type) +
         *     sizeof(Vector2) +
         *     sizeof(Shaders::PhongShader::Normal::Type);
         *
         * mesh.addVertexBufferStride(buffer, 35 + sizeof(Float),
         *          stride, Shaders::PhongShader::Position());
         *     .addVertexBufferStride(buffer, 35 + sizeof(Float) +
         *          sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2),
         *          stride, Shaders::PhongShader::Normal());
         * @endcode
         *
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addVertexBufferStride(), addVertexBuffer(), setPrimitive(),
         *      setVertexCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         */
        template<class ...T> inline Mesh& addInterleavedVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes) {
            addInterleavedVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), attributes...);
            return *this;
        }

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * See addInterleavedVertexBuffer() for more information.
         */
        template<UnsignedInt location, class T> inline Mesh& addVertexBufferStride(Buffer& buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>& attribute) {
            addInterleavedVertexBufferInternal(buffer, offset, stride, attribute);
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
         * behaves the same as setIndexBuffer(Buffer*, GLintptr, IndexType).
         * On OpenGL ES 2.0 this function behaves always as
         * setIndexBuffer(Buffer*, GLintptr, IndexType), as this functionality
         * is not available there.
         * @see setIndexCount(), MeshTools::compressIndices(),
         *      @fn_gl{BindVertexArray}, @fn_gl{BindBuffer} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @return Reference to self (for method chaining)
         *
         * Prefer to use setIndexBuffer(Buffer*, GLintptr, IndexType, UnsignedInt, UnsignedInt)
         * for better performance.
         * @see setIndexCount(), MeshTools::compressIndices(),
         *      @fn_gl{BindVertexArray}, @fn_gl{BindBuffer} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type) {
            return setIndexBuffer(buffer, offset, type, 0, 0);
        }

        /**
         * @brief Draw the mesh
         *
         * Expects an active shader with all uniforms set. See
         * @ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
         * for more information.
         * @see @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer}, @fn_gl{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray} (if @extension{APPLE,vertex_array_object}
         *      is available), @fn_gl{DrawArrays} or @fn_gl{DrawElements}/@fn_gl{DrawRangeElements}.
         */
        void draw() {
            #ifndef MAGNUM_TARGET_GLES2
            drawInternal(0, _vertexCount, _indexOffset, _indexCount, _indexStart, _indexEnd);
            #else
            drawInternal(0, _vertexCount, _indexOffset, _indexCount);
            #endif
        }

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
        };

        #ifndef MAGNUM_TARGET_GLES2
        struct MAGNUM_LOCAL IntegerAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
        };

        #ifndef MAGNUM_TARGET_GLES
        struct MAGNUM_LOCAL LongAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
        };
        #endif
        #endif
        #endif

        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        /* Adding non-interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> inline void addVertexBufferInternal(Buffer& buffer, GLintptr offset, const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, 0);

            /* Add size of this attribute array to offset for next attribute */
            addVertexBufferInternal(buffer, offset+attribute.dataSize()*_vertexCount, attributes...);
        }
        template<class ...T> inline void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addVertexBufferInternal(buffer, offset+gap, attributes...);
        }
        inline void addVertexBufferInternal(Buffer&, GLintptr) {}

        /* Computing stride of interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> inline static GLsizei strideOfInterleaved(const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            return attribute.dataSize() + strideOfInterleaved(attributes...);
        }
        template<class ...T> inline static GLsizei strideOfInterleaved(GLintptr gap, const T&... attributes) {
            return gap + strideOfInterleaved(attributes...);
        }
        inline static GLsizei strideOfInterleaved() { return 0; }

        /* Adding interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> inline void addInterleavedVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, stride);

            /* Add size of this attribute to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+attribute.dataSize(), stride, attributes...);
        }
        template<class ...T> inline void addInterleavedVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+gap, stride, attributes...);
        }
        inline void addInterleavedVertexBufferInternal(Buffer&, GLsizei, GLintptr) {}

        template<UnsignedInt location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::Type, Float>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            for(UnsignedInt i = 0; i != Implementation::Attribute<T>::vectorCount(); ++i)
                (this->*attributePointerImplementation)(Attribute{
                    &buffer,
                    location+i,
                    static_cast<GLint>(attribute.components()),
                    static_cast<GLenum>(attribute.dataType()),
                    bool(attribute.dataOptions() & AbstractShaderProgram::Attribute<location, T>::DataOption::Normalized),
                    offset,
                    stride
                });
        }

        #ifndef MAGNUM_TARGET_GLES2
        template<UnsignedInt location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::Type>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            (this->*attributeIPointerImplementation)(IntegerAttribute{
                &buffer,
                location,
                static_cast<GLint>(attribute.components()),
                static_cast<GLenum>(attribute.dataType()),
                offset,
                stride
            });
        }

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::Type, Double>::value, Buffer&>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            for(UnsignedInt i = 0; i != Implementation::Attribute<T>::vectorCount(); ++i)
                (this->*attributeLPointerImplementation)(LongAttribute{
                    &buffer,
                    location+i,
                    static_cast<GLint>(attribute.components()),
                    static_cast<GLenum>(attribute.dataType()),
                    offset,
                    stride
                });
        }
        #endif
        #endif

        static void MAGNUM_LOCAL bindVAO(GLuint vao);

        void MAGNUM_LOCAL vertexAttribPointer(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL vertexAttribPointer(const IntegerAttribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL vertexAttribPointer(const LongAttribute& attribute);
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void drawInternal(Int firstVertex, Int vertexCount, GLintptr indexOffset, Int indexCount, Int indexStart, Int indexEnd);
        #else
        void drawInternal(Int firstVertex, Int vertexCount, GLintptr indexOffset, Int indexCount);
        #endif

        typedef void(Mesh::*CreateImplementation)();
        void MAGNUM_LOCAL createImplementationDefault();
        void MAGNUM_LOCAL createImplementationVAO();
        static MAGNUM_LOCAL CreateImplementation createImplementation;

        typedef void(Mesh::*DestroyImplementation)();
        void MAGNUM_LOCAL destroyImplementationDefault();
        void MAGNUM_LOCAL destroyImplementationVAO();
        static MAGNUM_LOCAL DestroyImplementation destroyImplementation;

        typedef void(Mesh::*AttributePointerImplementation)(const Attribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const Attribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const Attribute& attribute);
        #endif
        static AttributePointerImplementation attributePointerImplementation;

        #ifndef MAGNUM_TARGET_GLES2
        typedef void(Mesh::*AttributeIPointerImplementation)(const IntegerAttribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const IntegerAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const IntegerAttribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const IntegerAttribute& attribute);
        #endif
        static AttributeIPointerImplementation attributeIPointerImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(Mesh::*AttributeLPointerImplementation)(const LongAttribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDSA(const LongAttribute& attribute);
        static AttributeLPointerImplementation attributeLPointerImplementation;
        #endif
        #endif

        typedef void(Mesh::*BindIndexBufferImplementation)(Buffer&);
        void MAGNUM_LOCAL bindIndexBufferImplementationDefault(Buffer& buffer);
        void MAGNUM_LOCAL bindIndexBufferImplementationVAO(Buffer& buffer);
        static MAGNUM_LOCAL BindIndexBufferImplementation bindIndexBufferImplementation;

        typedef void(Mesh::*BindImplementation)();
        void MAGNUM_LOCAL bindImplementationDefault();
        void MAGNUM_LOCAL bindImplementationVAO();
        static MAGNUM_LOCAL BindImplementation bindImplementation;

        typedef void(Mesh::*UnbindImplementation)();
        void MAGNUM_LOCAL unbindImplementationDefault();
        void MAGNUM_LOCAL unbindImplementationVAO();
        static MAGNUM_LOCAL UnbindImplementation unbindImplementation;

        GLuint _id;
        Primitive _primitive;
        Int _vertexCount, _indexCount;
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
Debug MAGNUM_EXPORT operator<<(Debug debug, Mesh::Primitive value);

/** @debugoperator{Magnum::Mesh} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Mesh::IndexType value);

template<class ...T> inline Mesh& Mesh::addVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes) {
    CORRADE_ASSERT(sizeof...(attributes) == 1 || _vertexCount != 0,
        "Mesh::addVertexBuffer(): vertex count must be set before binding attributes", *this);

    addVertexBufferInternal(buffer, offset, attributes...);
    return *this;
}

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Mesh} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::Mesh::Primitive> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::Mesh::Primitive value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns @ref Magnum::Mesh::Primitive::Points "Mesh::Primitive::Points".
     */
    static Magnum::Mesh::Primitive fromString(const std::string& stringValue, ConfigurationValueFlags);
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
