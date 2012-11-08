#ifndef Magnum_Trade_MeshData3D_h
#define Magnum_Trade_MeshData3D_h
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
 * @brief Class Magnum::Trade::MeshData3D
 */

#include <string>

#include "Math/Point3D.h"
#include "Mesh.h"

namespace Magnum { namespace Trade {

/**
@brief Three-dimensional mesh data

Provides access to mesh data and additional information, such as primitive
type.
@see MeshData2D
*/
class MAGNUM_EXPORT MeshData3D {
    MeshData3D(const MeshData3D& other) = delete;
    MeshData3D(MeshData3D&& other) = delete;
    MeshData3D& operator=(const MeshData3D& other) = delete;
    MeshData3D& operator=(MeshData3D&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param name              %Mesh name
         * @param primitive         Primitive
         * @param indices           Array with indices or 0, if this is not
         *      indexed mesh
         * @param positions         Array with vertex positions. At least one
         *      position array should be present.
         * @param normals           Array with normal arrays or empty array
         * @param textureCoords2D   Array with two-dimensional texture
         *      coordinate arrays or empty array
         */
        inline MeshData3D(const std::string& name, Mesh::Primitive primitive, std::vector<std::uint32_t>* indices, std::vector<std::vector<Point3D>*> positions, std::vector<std::vector<Vector3>*> normals, std::vector<std::vector<Vector2>*> textureCoords2D): _name(name), _primitive(primitive), _indices(indices), _positions(positions), _normals(normals), _textureCoords2D(textureCoords2D) {}

        /** @brief Destructor */
        ~MeshData3D();

        /** @brief %Mesh name */
        inline std::string name() const { return _name; }

        /** @brief Primitive */
        inline Mesh::Primitive primitive() const { return _primitive; }

        /**
         * @brief Indices
         * @return Indices or nullptr if the mesh is not indexed.
         */
        inline std::vector<std::uint32_t>* indices() { return _indices; }
        inline const std::vector<std::uint32_t>* indices() const { return _indices; } /**< @overload */

        /** @brief Count of vertex position arrays */
        inline std::uint32_t positionArrayCount() const { return _positions.size(); }

        /**
         * @brief Positions
         * @param id    ID of position data array
         * @return Positions or nullptr if there is no vertex array with given
         *      ID.
         */
        inline std::vector<Point3D>* positions(std::uint32_t id) { return _positions[id]; }
        inline const std::vector<Point3D>* positions(std::uint32_t id) const { return _positions[id]; } /**< @overload */

        /** @brief Count of normal arrays */
        inline std::uint32_t normalArrayCount() const { return _normals.size(); }

        /**
         * @brief Normals
         * @param id    ID of normal data array
         * @return Normals or nullptr if there is no normal array with given
         *      ID.
         */
        inline std::vector<Vector3>* normals(std::uint32_t id) { return _normals[id]; }
        inline const std::vector<Vector3>* normals(std::uint32_t id) const { return _normals[id]; } /**< @overload */

        /** @brief Count of 2D texture coordinate arrays */
        inline std::uint32_t textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    ID of texture coordinates array
         * @return %Texture coordinates or nullptr if there is no texture
         *      coordinates array with given ID.
         */
        inline std::vector<Vector2>* textureCoords2D(std::uint32_t id) { return _textureCoords2D[id]; }
        inline const std::vector<Vector2>* textureCoords2D(std::uint32_t id) const { return _textureCoords2D[id]; } /**< @overload */

    private:
        std::string _name;
        Mesh::Primitive _primitive;
        std::vector<std::uint32_t>* _indices;
        std::vector<std::vector<Point3D>*> _positions;
        std::vector<std::vector<Vector3>*> _normals;
        std::vector<std::vector<Vector2>*> _textureCoords2D;
};

}}

#endif
