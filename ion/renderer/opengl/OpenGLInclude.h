///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		RendererOpenGL.h
// Date:		13th June 2018
// Authors:		Matt Phillips
// Description:	OpenGL  API includes
///////////////////////////////////////////////////

#pragma once

#include <ion/core/Platform.h>

#include "OpenGLVersion.h"

#if defined ION_RENDER_SUPPORTS_GLEW
#include <GL/glew.h>
#endif

#if defined ION_RENDER_SUPPORTS_GLUT
#include <GL/glut.h>
#endif

#if defined ION_PLATFORM_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_OpenGL.h>
#elif defined ION_PLATFORM_LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif defined ION_PLATFORM_RASPBERRYPI
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif defined ION_PLATFORM_DREAMCAST
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glkos.h>
#elif defined ION_PLATFORM_SWITCH
#include <nn/gll.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#elif defined ION_PLATFORM_ANDROID
#include <EGL/egl.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include <EGL/eglext.h>
#pragma GCC diagnostic pop
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#if defined ION_PLATFORM_WINDOWS
#include <GL/wglext.h>
#endif

#if defined ION_RENDER_SUPPORTS_CGGL
#include "renderer/cggl/ShaderCgGL.h"
#endif

#if ION_PLATFORM_WINDOWS || defined ION_PLATFORM_SWITCH
#define ION_GL_SUPPORT_FENCE
#define ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
#endif
