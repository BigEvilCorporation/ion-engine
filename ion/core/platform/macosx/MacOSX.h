///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#pragma once

//Includes
#include <unistd.h>
#include <cstdio>
#include <sys/time.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include <signal.h>
#include <string.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <limits.h>

//C strings
#define sprintf_s snprintf
