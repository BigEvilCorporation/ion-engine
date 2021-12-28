///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		SDLInclude.h
// Date:		13th August 2018
// Authors:		Matt Phillips
// Description:	SDL/SDL2 API includes
///////////////////////////////////////////////////

#pragma once

#if defined ION_RENDER_SUPPORTS_SDL2
#include <SDL2/SDL.h>
#endif

#if defined ION_RENDER_SUPPORTS_SDL
#if defined ION_PLATFORM_DREAMCAST
//#include <SDL/SDL.h>
#else
#error PLATFORM_SDL_UNSUPPORTED
#endif
#endif

#if defined ION_RENDER_SUPPORTS_SDL2IMAGE
#if defined ION_PLATFORM_WINDOWS
#include <SDL2Image/SDL_image.h>
#elif defined ION_PLATFORM_LINUX
#include <SDL2/SDL_image.h>
#elif defined ION_PLATFORM_MACOSX
#include <SDL_image.h>
#endif
#endif

#if defined ION_RENDER_SUPPORTS_SDLIMAGE
#if defined ION_PLATFORM_DREAMCAST
#include <SDL_image.h>
#else
#error PLATFORM_SDL_UNSUPPORTED
#endif
#endif
