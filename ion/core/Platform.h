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
#include <sys/time.h>
#elif defined ION_PLATFORM_LINUX
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#elif defined ION_PLATFORM_RASPBERRYPI
#include <sys/time.h>
#elif defined ION_PLATFORM_DREAMCAST
#include <kos.h>
#include <time.h>
#else
#error Unsupported Platform
#endif

//Defines/undefs
#if defined ION_PLATFORM_WINDOWS

#ifdef DELETE
#undef DELETE
#endif

#ifdef MessageBox
#undef MessageBox
#endif

#ifdef Yield
#undef Yield
#endif

#endif
