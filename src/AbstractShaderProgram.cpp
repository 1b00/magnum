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

#include "AbstractShaderProgram.h"

#include <fstream>

#include "Math/Matrix.h"
#include "Shader.h"
#include "Implementation/State.h"
#include "Implementation/ShaderProgramState.h"
#include "Extensions.h"

#define LINKER_MESSAGE_MAX_LENGTH 1024

using namespace std;

namespace Magnum {

AbstractShaderProgram::Uniform1fImplementation AbstractShaderProgram::uniform1fImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2fvImplementation AbstractShaderProgram::uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3fvImplementation AbstractShaderProgram::uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4fvImplementation AbstractShaderProgram::uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform1iImplementation AbstractShaderProgram::uniform1iImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2ivImplementation AbstractShaderProgram::uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3ivImplementation AbstractShaderProgram::uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4ivImplementation AbstractShaderProgram::uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#ifndef MAGNUM_TARGET_GLES2
AbstractShaderProgram::Uniform1uiImplementation AbstractShaderProgram::uniform1uiImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2uivImplementation AbstractShaderProgram::uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3uivImplementation AbstractShaderProgram::uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4uivImplementation AbstractShaderProgram::uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif
#ifndef MAGNUM_TARGET_GLES
AbstractShaderProgram::Uniform1dImplementation AbstractShaderProgram::uniform1dImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2dvImplementation AbstractShaderProgram::uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3dvImplementation AbstractShaderProgram::uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4dvImplementation AbstractShaderProgram::uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif

AbstractShaderProgram::UniformMatrix2fvImplementation AbstractShaderProgram::uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3fvImplementation AbstractShaderProgram::uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4fvImplementation AbstractShaderProgram::uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#ifndef MAGNUM_TARGET_GLES2
AbstractShaderProgram::UniformMatrix2x3fvImplementation AbstractShaderProgram::uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x2fvImplementation AbstractShaderProgram::uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x4fvImplementation AbstractShaderProgram::uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x2fvImplementation AbstractShaderProgram::uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x4fvImplementation AbstractShaderProgram::uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x3fvImplementation AbstractShaderProgram::uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif
#ifndef MAGNUM_TARGET_GLES
AbstractShaderProgram::UniformMatrix2dvImplementation AbstractShaderProgram::uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3dvImplementation AbstractShaderProgram::uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4dvImplementation AbstractShaderProgram::uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x3dvImplementation AbstractShaderProgram::uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x2dvImplementation AbstractShaderProgram::uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x4dvImplementation AbstractShaderProgram::uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x2dvImplementation AbstractShaderProgram::uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x4dvImplementation AbstractShaderProgram::uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x3dvImplementation AbstractShaderProgram::uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif

GLint AbstractShaderProgram::maxSupportedVertexAttributeCount() {
    GLint& value = Context::current()->state()->shaderProgram->maxSupportedVertexAttributeCount;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);

    return value;
}

AbstractShaderProgram::~AbstractShaderProgram() {
    /* Remove current usage from the state */
    GLuint& current = Context::current()->state()->shaderProgram->current;
    if(current == _id) current = 0;

    glDeleteProgram(_id);
}

bool AbstractShaderProgram::use() {
    if(state != Linked) return false;

    /* Use only if the program isn't already in use */
    GLuint& current = Context::current()->state()->shaderProgram->current;
    if(current != _id) glUseProgram(current = _id);
    return true;
}

bool AbstractShaderProgram::attachShader(Shader& shader) {
    GLuint _shader = shader.compile();
    if(!_shader) return false;

    glAttachShader(_id, _shader);
    return true;
}

void AbstractShaderProgram::bindAttributeLocation(GLuint location, const string& name) {
    CORRADE_ASSERT(state == Initialized, "AbstractShaderProgram: attribute cannot be bound after linking.", );

    glBindAttribLocation(_id, location, name.c_str());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::bindFragmentDataLocation(GLuint location, const std::string& name) {
    CORRADE_ASSERT(state == Initialized, "AbstractShaderProgram: fragment data location cannot be bound after linking.", );

    glBindFragDataLocation(_id, location, name.c_str());
}
void AbstractShaderProgram::bindFragmentDataLocationIndexed(GLuint location, GLuint index, const std::string& name) {
    CORRADE_ASSERT(state == Initialized, "AbstractShaderProgram: fragment data location cannot be bound after linking.", );

    glBindFragDataLocationIndexed(_id, location, index, name.c_str());
}
#endif

void AbstractShaderProgram::link() {
    /* Already compiled or failed, exit */
    if(state != Initialized) return;

    /* Link shader program */
    glLinkProgram(_id);

    /* Check link status */
    GLint status;
    glGetProgramiv(_id, GL_LINK_STATUS, &status);

    /* Display errors or warnings */
    char message[LINKER_MESSAGE_MAX_LENGTH];
    glGetProgramInfoLog(_id, LINKER_MESSAGE_MAX_LENGTH, nullptr, message);

    /* Show error log and delete shader */
    if(status == GL_FALSE) {
        Error() << "AbstractShaderProgram: linking failed with the following message:\n"
                << message;

    /* Or just warnings, if there are any */
    } else if(message[0] != 0) {
        Debug() << "AbstractShaderProgram: linking succeeded with the following message:\n"
                << message;
    }

    state = status == GL_FALSE ? Failed : Linked;
}

GLint AbstractShaderProgram::uniformLocation(const std::string& name) {
    /** @todo What if linking just failed (not programmer error?) */
    CORRADE_ASSERT(state == Linked, "AbstractShaderProgram: uniform location cannot be retrieved before linking.", -1);

    GLint location = glGetUniformLocation(_id, name.c_str());
    if(location == -1)
        Warning() << "AbstractShaderProgram: location of uniform \'" + name + "\' cannot be retrieved!";
    return location;
}

void AbstractShaderProgram::initializeContextBasedFunctionality(Context* context) {
    /** @todo OpenGL ES 2 has extension @es_extension{EXT,separate_shader_objects} for this */
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ||
       context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractShaderProgram: using" << (context->isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ?
            Extensions::GL::ARB::separate_shader_objects::string() : Extensions::GL::EXT::direct_state_access::string()) << "features";
        uniform1fImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1iImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1uiImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1dImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
    }
    #else
    static_cast<void>(context);
    #endif
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, GLfloat value) {
    use();
    glUniform1f(location, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, GLfloat value) {
    glProgramUniform1f(_id, location, value);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<2, GLfloat>& value) {
    use();
    glUniform2fv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<2, GLfloat>& value) {
    glProgramUniform2fv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<3, GLfloat>& value) {
    use();
    glUniform3fv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<3, GLfloat>& value) {
    glProgramUniform3fv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<4, GLfloat>& value) {
    use();
    glUniform4fv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<4, GLfloat>& value) {
    glProgramUniform4fv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, GLint value) {
    use();
    glUniform1i(location, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, GLint value) {
    glProgramUniform1i(_id, location, value);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<2, GLint>& value) {
    use();
    glUniform2iv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<2, GLint>& value) {
    glProgramUniform2iv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<3, GLint>& value) {
    use();
    glUniform3iv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<3, GLint>& value) {
    glProgramUniform3iv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<4, GLint>& value) {
    use();
    glUniform4iv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<4, GLint>& value) {
    glProgramUniform4iv(_id, location, 1, value.data());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationDefault(GLint location, GLuint value) {
    use();
    glUniform1ui(location, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, GLuint value) {
    glProgramUniform1ui(_id, location, value);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<2, GLuint>& value) {
    use();
    glUniform2uiv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<2, GLuint>& value) {
    glProgramUniform2uiv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<3, GLuint>& value) {
    use();
    glUniform3uiv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<3, GLuint>& value) {
    glProgramUniform3uiv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<4, GLuint>& value) {
    use();
    glUniform4uiv(location, 1, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<4, GLuint>& value) {
    glProgramUniform4uiv(_id, location, 1, value.data());
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDefault(GLint location, GLdouble value) {
    use();
    glUniform1d(location, value);
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, GLdouble value) {
    glProgramUniform1d(_id, location, value);
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<2, GLdouble>& value) {
    use();
    glUniform2dv(location, 1, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<2, GLdouble>& value) {
    glProgramUniform2dv(_id, location, 1, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<3, GLdouble>& value) {
    use();
    glUniform3dv(location, 1, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<3, GLdouble>& value) {
    glProgramUniform3dv(_id, location, 1, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Vector<4, GLdouble>& value) {
    use();
    glUniform4dv(location, 1, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Vector<4, GLdouble>& value) {
    glProgramUniform4dv(_id, location, 1, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<2, GLfloat>& value) {
    use();
    glUniformMatrix2fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<2, GLfloat>& value) {
    glProgramUniformMatrix2fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<3, GLfloat>& value) {
    use();
    glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<3, GLfloat>& value) {
    glProgramUniformMatrix3fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<4, GLfloat>& value) {
    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<4, GLfloat>& value) {
    glProgramUniformMatrix4fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value) {
    use();
    glUniformMatrix2x3fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value) {
    glProgramUniformMatrix2x3fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value) {
    use();
    glUniformMatrix3x2fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value) {
    glProgramUniformMatrix3x2fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value) {
    use();
    glUniformMatrix2x4fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value) {
    glProgramUniformMatrix2x4fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value) {
    use();
    glUniformMatrix4x2fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value) {
    glProgramUniformMatrix4x2fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value) {
    use();
    glUniformMatrix3x4fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value) {
    glProgramUniformMatrix3x4fv(_id, location, 1, GL_FALSE, value.data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value) {
    use();
    glUniformMatrix4x3fv(location, 1, GL_FALSE, value.data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value) {
    glProgramUniformMatrix4x3fv(_id, location, 1, GL_FALSE, value.data());
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<2, GLdouble>& value) {
    use();
    glUniformMatrix2dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<2, GLdouble>& value) {
    glProgramUniformMatrix2dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<3, GLdouble>& value) {
    use();
    glUniformMatrix3dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<3, GLdouble>& value) {
    glProgramUniformMatrix3dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::Matrix<4, GLdouble>& value) {
    use();
    glUniformMatrix4dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::Matrix<4, GLdouble>& value) {
    glProgramUniformMatrix4dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 3, GLdouble>& value) {
    use();
    glUniformMatrix2x3dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 3, GLdouble>& value) {
    glProgramUniformMatrix2x3dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 2, GLdouble>& value) {
    use();
    glUniformMatrix3x2dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 2, GLdouble>& value) {
    glProgramUniformMatrix3x2dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 4, GLdouble>& value) {
    use();
    glUniformMatrix2x4dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 4, GLdouble>& value) {
    glProgramUniformMatrix2x4dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 2, GLdouble>& value) {
    use();
    glUniformMatrix4x2dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 2, GLdouble>& value) {
    glProgramUniformMatrix4x2dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 4, GLdouble>& value) {
    use();
    glUniformMatrix3x4dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 4, GLdouble>& value) {
    glProgramUniformMatrix3x4dv(_id, location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 3, GLdouble>& value) {
    use();
    glUniformMatrix4x3dv(location, 1, GL_FALSE, value.data());
}

void AbstractShaderProgram::uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 3, GLdouble>& value) {
    glProgramUniformMatrix4x3dv(_id, location, 1, GL_FALSE, value.data());
}
#endif

}
