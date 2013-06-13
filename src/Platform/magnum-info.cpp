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

#include <Utility/Debug.h>
#include <corradeCompatibility.h>
#ifdef CORRADE_TARGET_NACL
#include <Utility/NaClStreamBuffer.h>
#endif

#include "Context.h"
#ifndef CORRADE_TARGET_NACL
#include "Platform/WindowlessGlxApplication.h"
#else
#include "Platform/WindowlessNaClApplication.h"
#endif

namespace Magnum {

class MagnumInfo: public Platform::WindowlessApplication {
    public:
        explicit MagnumInfo(const Arguments& arguments);

        int exec() override { return 0; }
};

MagnumInfo::MagnumInfo(const Arguments& arguments): Platform::WindowlessApplication(arguments) {
    Context* c = Context::current();

    /* Pass debug output as messages to JavaScript */
    #ifdef CORRADE_TARGET_NACL
    Utility::NaClMessageStreamBuffer buffer(this);
    std::ostream out(&buffer);
    Debug::setOutput(&out);
    #endif

    Debug() << "";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "  | Information about Magnum engine and OpenGL capabilities |";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "";

    Debug() << "Used application: Platform::WindowlessGlxApplication";
    Debug() << "Compilation flags:";
    #ifdef CORRADE_GCC44_COMPATIBILITY
    Debug() << "    CORRADE_GCC44_COMPATIBILITY";
    #endif
    #ifdef CORRADE_GCC45_COMPATIBILITY
    Debug() << "    CORRADE_GCC45_COMPATIBILITY";
    #endif
    #ifdef CORRADE_GCC46_COMPATIBILITY
    Debug() << "    CORRADE_GCC46_COMPATIBILITY";
    #endif
    #ifdef CORRADE_GCC47_COMPATIBILITY
    Debug() << "    CORRADE_GCC47_COMPATIBILITY";
    #endif
    #ifdef CORRADE_BUILD_STATIC
    Debug() << "    CORRADE_BUILD_STATIC";
    #endif
    #ifdef CORRADE_TARGET_NACL
    Debug() << "    CORRADE_TARGET_NACL";
    #endif
    #ifdef CORRADE_TARGET_NACL_NEWLIB
    Debug() << "    CORRADE_TARGET_NACL_NEWLIB";
    #endif
    #ifdef CORRADE_TARGET_NACL_GLIBC
    Debug() << "    CORRADE_TARGET_NACL_GLIBC";
    #endif
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    Debug() << "    CORRADE_TARGET_EMSCRIPTEN";
    #endif
    #ifdef MAGNUM_BUILD_STATIC
    Debug() << "    MAGNUM_BUILD_STATIC";
    #endif
    #ifdef MAGNUM_TARGET_GLES
    Debug() << "    MAGNUM_TARGET_GLES";
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Debug() << "    MAGNUM_TARGET_GLES2";
    #endif
    #ifdef MAGNUM_TARGET_DESKTOP_GLES
    Debug() << "    MAGNUM_TARGET_DESKTOP_GLES";
    #endif
    Debug() << "";

    Debug() << "Vendor:" << c->vendorString();
    Debug() << "Renderer:" << c->rendererString();
    Debug() << "OpenGL version:" << c->version() << '(' + c->versionString() + ')';

    Debug() << "Supported GLSL versions:";
    const std::vector<std::string> shadingLanguageVersions = c->shadingLanguageVersionStrings();
    for(auto it = shadingLanguageVersions.begin(); it != shadingLanguageVersions.end(); ++it)
        Debug() << "   " << *it;

    Debug() << "";

    /* Get first future (not supported) version */
    std::vector<Version> versions{
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        #else
        Version::GLES200,
        Version::GLES300,
        #endif
        Version::None
    };
    std::size_t future = 0;
    while(versions[future] != Version::None && c->isVersionSupported(versions[future]))
        ++future;

    /* Display supported OpenGL extensions from unsupported versions */
    for(std::size_t i = future; i != versions.size(); ++i) {
        if(versions[i] != Version::None)
            Debug() << versions[i] << "extension support:";
        else Debug() << "Vendor extension support:";

        auto extensions = Extension::extensions(versions[i]);
        for(auto it = extensions.begin(); it != extensions.end(); ++it) {
            const auto& extension = *it;

            std::string extensionName = extension.string();
            Debug d;
            d << "   " << extensionName << std::string(60-extensionName.size(), ' ');
            if(c->isExtensionSupported(extension))
                d << "SUPPORTED";
            else if(c->isVersionSupported(extension.requiredVersion()))
                d << "    -";
            else
                d << "   ---";
        }

        Debug() << "";
    }
}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::MagnumInfo)
