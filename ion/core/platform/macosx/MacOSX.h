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

//C strings
#define sprintf_s snprintf
