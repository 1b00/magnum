#ifndef Magnum_Trade_SceneData_h
#define Magnum_Trade_SceneData_h
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
 * @brief Class Magnum::Trade::SceneData
 */

#include <string>
#include <vector>

namespace Magnum { namespace Trade {

/**
@brief %Scene data
*/
class MAGNUM_EXPORT SceneData {
    SceneData(const SceneData& other) = delete;
    SceneData(SceneData&& other) = delete;
    SceneData& operator=(const SceneData& other) = delete;
    SceneData& operator=(SceneData&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param name          Scene name
         * @param children2D    Two-dimensional child objects
         * @param children3D    Three-dimensional child objects
         */
        inline SceneData(const std::string& name, const std::vector<std::uint32_t>& children2D, const std::vector<std::uint32_t>& children3D): _name(name), _children2D(children2D), _children3D(children3D) {}

        /** @brief Scene name */
        inline std::string name() const { return _name; }

        /** @brief Two-dimensional child objects */
        inline const std::vector<std::uint32_t>& children2D() const { return _children2D; }

        /** @brief Three-dimensional child objects */
        inline const std::vector<std::uint32_t>& children3D() const { return _children3D; }

    private:
        std::string _name;
        std::vector<std::uint32_t> _children2D,
            _children3D;
};

}}

#endif
