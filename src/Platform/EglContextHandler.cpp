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

#include "EglContextHandler.h"

#include <Utility/Debug.h>

#include "Context.h"

namespace Magnum { namespace Platform {

EglContextHandler::~EglContextHandler() {
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
}

VisualId EglContextHandler::getVisualId(EGLNativeDisplayType nativeDisplay) {
    /* Initialize */
    display = eglGetDisplay(nativeDisplay);
    if(!eglInitialize(display, nullptr, nullptr)) {
        Error() << "Cannot initialize EGL:" << errorString(eglGetError());
        std::exit(1);
    }

    #ifndef MAGNUM_TARGET_GLES
    EGLenum api = EGL_OPENGL_API;
    #else
    EGLenum api = EGL_OPENGL_ES_API;
    #endif
    if(!eglBindAPI(api)) {
        Error() << "Cannot bind EGL API:" << errorString(eglGetError());
        std::exit(1);
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #endif
        EGL_NONE
    };
    EGLint configCount;
    if(!eglChooseConfig(display, attribs, &config, 1, &configCount)) {
        Error() << "Cannot get EGL visual config:" << errorString(eglGetError());
        std::exit(1);
    }

    if(!configCount) {
        Error() << "No matching EGL visual config available";
        std::exit(1);
    }

    /* Get visual ID */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID:" << errorString(eglGetError());
        std::exit(1);
    }

    return visualId;
}

void EglContextHandler::createContext(EGLNativeWindowType window) {
    static const EGLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        EGL_CONTEXT_CLIENT_VERSION, 2,
        #endif
        EGL_NONE
    };
    if(!eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes)) {
        Error() << "Cannot create EGL context:" << errorString(eglGetError());
        std::exit(1);
    }
    if(!(surface = eglCreateWindowSurface(display, config, window, NULL))) {
        Error() << "Cannot create window surface:" << errorString(eglGetError());
        std::exit(1);
    }

    /** @bug Fixme: On desktop OpenGL and Mesa EGL implementation OpenGL version is 1.0, which is wrong */
}

const char* EglContextHandler::errorString(EGLint error) {
    switch(error) {
        #define _error(name) case name: return #name;
        _error(EGL_SUCCESS)
        _error(EGL_NOT_INITIALIZED)
        _error(EGL_BAD_ACCESS)
        _error(EGL_BAD_ALLOC)
        _error(EGL_BAD_ATTRIBUTE)
        _error(EGL_BAD_CONTEXT)
        _error(EGL_BAD_CONFIG)
        _error(EGL_BAD_CURRENT_SURFACE)
        _error(EGL_BAD_DISPLAY)
        _error(EGL_BAD_SURFACE)
        _error(EGL_BAD_MATCH)
        _error(EGL_BAD_PARAMETER)
        _error(EGL_BAD_NATIVE_PIXMAP)
        _error(EGL_BAD_NATIVE_WINDOW)
        _error(EGL_CONTEXT_LOST)
        #undef _error
    }

    return {};
}

}}
