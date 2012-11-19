#ifndef Magnum_Shaders_PhongShader_h
#define Magnum_Shaders_PhongShader_h
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
 * @brief Class Magnum::Shaders::PhongShader
 */

#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong shader

If supported, uses GLSL 3.20 and @extension{ARB,explicit_attrib_location},
otherwise falls back to GLSL 1.20.
*/
class SHADERS_EXPORT PhongShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Point3D> Position; /**< @brief Vertex position */
        typedef Attribute<1, Vector3> Normal;   /**< @brief Normal direction */

        PhongShader();

        /**
         * @brief Set ambient color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(0.0f, 0.0f, 0.0f)`.
         */
        inline PhongShader* setAmbientColor(const Color3<GLfloat>& color) {
            setUniform(ambientColorUniform, color);
            return this;
        }

        /**
         * @brief Set diffuse color
         * @return Pointer to self (for method chaining)
         */
        inline PhongShader* setDiffuseColor(const Color3<GLfloat>& color) {
            setUniform(diffuseColorUniform, color);
            return this;
        }

        /**
         * @brief Set specular color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`.
         */
        inline PhongShader* setSpecularColor(const Color3<GLfloat>& color) {
            setUniform(specularColorUniform, color);
            return this;
        }

        /**
         * @brief Set shininess
         * @return Pointer to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * If not set, default value is `80.0f`.
         */
        inline PhongShader* setShininess(GLfloat shininess) {
            setUniform(shininessUniform, shininess);
            return this;
        }

        /**
         * @brief Set transformation matrix and normal matrix
         * @return Pointer to self (for method chaining)
         */
        inline PhongShader* setTransformation(const Matrix4& matrix) {
            setUniform(transformationMatrixUniform, matrix);
            setUniform(normalMatrixUniform, matrix.rotation());
            return this;
        }

        /**
         * @brief Set projection matrix
         * @return Pointer to self (for method chaining)
         */
        inline PhongShader* setProjection(const Matrix4& matrix) {
            setUniform(projectionMatrixUniform, matrix);
            return this;
        }

        /**
         * @brief Set light position
         * @return Pointer to self (for method chaining)
         */
        inline PhongShader* setLightPosition(const Vector3& light) {
            setUniform(lightUniform, light);
            return this;
        }

        /**
         * @brief Set light color
         * @return Pointer to self (for method chaining)
         *
         * If not set, default value is `(1.0f, 1.0f, 1.0f)`.
         */
        inline PhongShader* setLightColor(const Color3<GLfloat>& color) {
            setUniform(lightColorUniform, color);
            return this;
        }

    private:
        GLint ambientColorUniform,
            diffuseColorUniform,
            specularColorUniform,
            shininessUniform,
            transformationMatrixUniform,
            projectionMatrixUniform,
            normalMatrixUniform,
            lightUniform,
            lightColorUniform;
};

}}

#endif
