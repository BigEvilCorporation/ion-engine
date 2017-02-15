///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#pragma once

//Includes
#if defined ION_PLATFORM_WINDOWS
#include <Windows.h>
#include <direct.h>
#elif defined ION_PLATFORM_MACOSX
#include <kos.h>
#include <sys/time.h>
#elif defined ION_PLATFORM_DREAMCAST
#include <time.h>
#elif defined ION_PLATFORM_LINUX

#else
#error Unsupported Platform
#endif

//Defines/undefs
#if defined ION_PLATFORM_WINDOWS
#if defined DELETE
#undef DELETE
#endif
#endif
