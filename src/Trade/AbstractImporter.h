#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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
 * @brief Class Magnum::Trade::AbstractImporter
 */

#include <Containers/EnumSet.h>
#include <PluginManager/Plugin.h>

#include "ImageData.h"

namespace Magnum { namespace Trade {

class AbstractMaterialData;
class CameraData;
class LightData;
class MeshData;
class ObjectData;
class SceneData;
class TextureData;

/**
@brief Base class for importer plugins

Importer is used for importing data like scenes, lights, objects, images,
textures etc.

@section AbstractImporterSubclassing Subclassing
Plugin implements function features(), one or more open() functions,
function close() and one or more pairs of data access functions, based on
which features are supported in given format.

For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.
*/
class MAGNUM_EXPORT AbstractImporter: public Corrade::PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImporter/0.2")

    public:
        /**
         * @brief Features supported by this importer
         *
         * @see Features, features()
         */
        enum class Feature {
            OpenFile = 0x01,    /**< Can open files specified by filename */
            OpenStream = 0x02   /**< Can open files from input streams */
        };

        /** @brief Set of features supported by this importer */
        typedef Corrade::Containers::EnumSet<Feature, int> Features;

        /** @brief Constructor */
        inline AbstractImporter(Corrade::PluginManager::AbstractPluginManager* manager = nullptr, const std::string& plugin = ""): Plugin(manager, plugin) {}

        /** @brief Features supported by this importer */
        virtual Features features() const = 0;

        /**
         * @brief Open file
         * @param filename  Filename
         * @return Whether the file was successfully opened
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. See also @ref Feature "Feature::OpenFile". Default
         * implementation prints message to error output and returns false.
         */
        virtual bool open(const std::string& filename);

        /**
         * @brief Open stream
         * @param in        Input stream
         * @return Whether the file was successfully opened
         *
         * See also open(const std::string&), @ref Feature
         * "Feature::OpenStream". Default implementation prints message to
         * error output and returns false.
         */
        virtual bool open(std::istream& in);

        /** @brief Close file */
        virtual void close() = 0;

        /** @{ @name Data accessors
         * Each function pair provides access to the data.
         */

        /**
         * @brief Default scene
         *
         * When there is more than one scene, returns ID of the default one.
         * If there is no default scene, returns -1.
         *
         * @note The function is not const, because the value will probably
         *      be lazy-populated.
         */
        virtual inline int defaultScene() { return -1; }

        /** @brief %Scene count */
        virtual inline unsigned int sceneCount() const { return 0; }

        /**
         * @brief %Scene ID for given name
         *
         * If no scene for given name exists, returns -1.
         * @see SceneData::name()
         */
        virtual int sceneForName(const std::string& name);

        /**
         * @brief %Scene
         * @param id        %Scene ID, from range [0, sceneCount()).
         *
         * Returns pointer to given scene or nullptr, if no such scene exists.
         */
        virtual SceneData* scene(unsigned int id);

        /** @brief %Light count */
        virtual inline unsigned int lightCount() const { return 0; }

        /**
         * @brief %Light ID for given name
         *
         * If no light for given name exists, returns -1.
         * @see LightData::name()
         */
        virtual int lightForName(const std::string& name);

        /**
         * @brief %Light
         * @param id        %Light ID, from range [0, lightCount()).
         *
         * Returns pointer to given light or nullptr, if no such light exists.
         */
        virtual LightData* light(unsigned int id);

        /** @brief %Camera count */
        virtual inline unsigned int cameraCount() const { return 0; }

        /**
         * @brief %Camera ID for given name
         *
         * If no camera for given name exists, returns -1.
         * @see CameraData::name()
         */
        virtual int cameraForName(const std::string& name);

        /**
         * @brief %Camera
         * @param id        %Camera ID, from range [0, cameraCount()).
         *
         * Returns pointer to given camera or nullptr, if no such camera
         * exists.
         */
        virtual CameraData* camera(unsigned int id);

        /** @brief %Object count */
        virtual inline unsigned int objectCount() const { return 0; }

        /**
         * @brief %Object ID for given name
         *
         * If no scene for given name exists, returns -1.
         * @see ObjectData::name()
         */
        virtual int objectForName(const std::string& name);

        /**
         * @brief %Object
         * @param id        %Object ID, from range [0, objectCount()).
         *
         * Returns pointer to given object or nullptr, if no such object
         * exists.
         */
        virtual ObjectData* object(unsigned int id);

        /** @brief %Mesh count */
        virtual inline unsigned int meshCount() const { return 0; }

        /**
         * @brief %Mesh ID for given name
         *
         * If no mesh for given name exists, returns -1.
         * @see MeshData::name()
         */
        virtual int meshForName(const std::string& name);

        /**
         * @brief %Mesh
         * @param id        %Mesh ID, from range [0, meshCount()).
         *
         * Returns pointer to given mesh or nullptr, if no such mesh exists.
         */
        virtual MeshData* mesh(unsigned int id);

        /** @brief Material count */
        virtual inline unsigned int materialCount() const { return 0; }

        /**
         * @brief Material ID for given name
         *
         * If no material for given name exists, returns -1.
         * @see AbstractMaterialData::name()
         */
        virtual int materialForName(const std::string& name);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, materialCount()).
         *
         * Returns pointer to given material or nullptr, if no such material
         * exists.
         */
        virtual AbstractMaterialData* material(unsigned int id);

        /** @brief %Texture count */
        virtual inline unsigned int textureCount() const { return 0; }

        /**
         * @brief %Texture ID for given name
         *
         * If no texture for given name exists, returns -1.
         * @see TextureData::name()
         */
        virtual int textureForName(const std::string& name);

        /**
         * @brief %Texture
         * @param id        %Texture ID, from range [0, textureCount()).
         *
         * Returns pointer to given texture or nullptr, if no such texture
         * exists.
         */
        virtual TextureData* texture(unsigned int id);

        /** @brief One-dimensional image count */
        virtual inline unsigned int image1DCount() const { return 0; }

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see ImageData1D::name()
         */
        virtual int image1DForName(const std::string& name);

        /**
         * @brief One-dimensional image
         * @param id        %Image ID, from range [0, image1DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData1D* image1D(unsigned int id);

        /** @brief Two-dimensional image count */
        virtual inline unsigned int image2DCount() const { return 0; }

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see ImageData2D::name()
         */
        virtual int image2DForName(const std::string& name);

        /**
         * @brief Two-dimensional image
         * @param id        %Image ID, from range [0, image2DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData2D* image2D(unsigned int id);

        /** @brief Three-dimensional image count */
        virtual inline unsigned int image3DCount() const { return 0; }

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see ImageData3D::name()
         */
        virtual int image3DForName(const std::string& name);

        /**
         * @brief Three-dimensional image
         * @param id        %Image ID, from range [0, image3DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData3D* image3D(unsigned int id);

        /*@}*/
};

CORRADE_ENUMSET_OPERATORS(AbstractImporter::Features)

/* Implementations for inline functions with unused parameters */
inline int AbstractImporter::sceneForName(const std::string&) { return -1; }
inline SceneData* AbstractImporter::scene(unsigned int) { return nullptr; }
inline int AbstractImporter::lightForName(const std::string&) { return -1; }
inline LightData* AbstractImporter::light(unsigned int) { return nullptr; }
inline int AbstractImporter::cameraForName(const std::string&) { return -1; }
inline CameraData* AbstractImporter::camera(unsigned int) { return nullptr; }
inline int AbstractImporter::objectForName(const std::string&) { return -1; }
inline ObjectData* AbstractImporter::object(unsigned int) { return nullptr; }
inline int AbstractImporter::meshForName(const std::string&) { return -1; }
inline MeshData* AbstractImporter::mesh(unsigned int) { return nullptr; }
inline int AbstractImporter::materialForName(const std::string&) { return -1; }
inline AbstractMaterialData* AbstractImporter::material(unsigned int) { return nullptr; }
inline int AbstractImporter::textureForName(const std::string&) { return -1; }
inline TextureData* AbstractImporter::texture(unsigned int) { return nullptr; }
inline int AbstractImporter::image1DForName(const std::string&) { return -1; }
inline ImageData1D* AbstractImporter::image1D(unsigned int) { return nullptr; }
inline int AbstractImporter::image2DForName(const std::string&) { return -1; }
inline ImageData2D* AbstractImporter::image2D(unsigned int) { return nullptr; }
inline int AbstractImporter::image3DForName(const std::string&) { return -1; }
inline ImageData3D* AbstractImporter::image3D(unsigned int) { return nullptr; }

}}

#endif
