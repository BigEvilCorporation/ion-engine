///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#pragma once

//TODO - deprecate this
#include "platform/Platform.h"

//Includes
#if defined ION_PLATFORM_WINDOWS
#elif defined ION_PLATFORM_MACOSX
#elif defined ION_PLATFORM_LINUX
#elif defined ION_PLATFORM_RASPBERRYPI
#include <sys/time.h>
#include <signal.h>
#elif defined ION_PLATFORM_DREAMCAST
#else
#error Unsupported Platform
#endif
