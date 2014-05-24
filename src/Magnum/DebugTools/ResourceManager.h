#ifndef Magnum_DebugTools_ResourceManager_h
#define Magnum_DebugTools_ResourceManager_h
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
 * @brief Class @ref Magnum::DebugTools::ResourceManager
 */

#ifndef MAGNUM_RESOURCEMANAGER_DEFINE_INTERNALINSTANCE
#define MAGNUM_RESOURCEMANAGER_DONT_DEFINE_INTERNALINSTANCE
#endif
#include "Magnum/ResourceManager.h"

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/SceneGraph/SceneGraph.h"
#include "Magnum/Shapes/Shapes.h"

#ifdef CORRADE_MSVC2013_COMPATIBILITY
#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/MeshView.h"
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/DebugTools/ShapeRenderer.h"
#endif

namespace Magnum {

/** @todo Do the listing in one place, not five thousand! */

#ifndef CORRADE_TARGET_WINDOWS
extern template ResourceManager<AbstractShaderProgram, Buffer, Mesh, MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions> MAGNUM_DEBUGTOOLS_EXPORT *& ResourceManager<AbstractShaderProgram, Buffer, Mesh, MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions>::internalInstance();
#else
extern template class MAGNUM_DEBUGTOOLS_EXPORT ResourceManager<AbstractShaderProgram, Buffer, Mesh, MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions>;
#endif

namespace DebugTools {

/**
@brief %Resource manager for debug tools

Stores various data used by debug renderers. See @ref debug-tools for more
information.
*/
class MAGNUM_DEBUGTOOLS_EXPORT ResourceManager: public Magnum::ResourceManager<AbstractShaderProgram, Buffer, Mesh, MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions> {
    public:
        explicit ResourceManager();
        ~ResourceManager();
};

}}

#endif
