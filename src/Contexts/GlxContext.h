#ifndef Magnum_Contexts_GlxContext_h
#define Magnum_Contexts_GlxContext_h
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
 * @brief Class Magnum::Contexts::GlxContext
 */

#include "AbstractXContext.h"
#include "GlxInterface.h"

namespace Magnum { namespace Contexts {

/**
@brief GLX context

Uses GlxInterface.
*/
class GlxContext: public AbstractXContext {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates window with double-buffered OpenGL 3.3 core context or
         * OpenGL ES 2.0 context, if targetting OpenGL ES.
         */
        inline GlxContext(int& argc, char** argv, const std::string& title = "Magnum GLX context", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600)): AbstractXContext(new GlxInterface, argc, argv, title, size) {}
};

}}

#endif
