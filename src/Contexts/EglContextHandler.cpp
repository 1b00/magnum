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

#include "EglContextHandler.h"

#include <Utility/Debug.h>

#include "Context.h"

namespace Magnum { namespace Contexts {

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
        exit(1);
    }

    #ifndef MAGNUM_TARGET_GLES
    EGLenum api = EGL_OPENGL_API;
    #else
    EGLenum api = EGL_OPENGL_ES_API;
    #endif
    if(!eglBindAPI(api)) {
        Error() << "Cannot bind EGL API:" << errorString(eglGetError());
        exit(1);
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
        exit(1);
    }

    if(!configCount) {
        Error() << "No matching EGL visual config available";
        exit(1);
    }

    /* Get visual ID */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID:" << errorString(eglGetError());
        exit(1);
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
        exit(1);
    }
    if(!(surface = eglCreateWindowSurface(display, config, window, NULL))) {
        Error() << "Cannot create window surface:" << errorString(eglGetError());
        exit(1);
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

    return "";
}

}}
