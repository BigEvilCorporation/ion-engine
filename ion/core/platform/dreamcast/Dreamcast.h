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
#include <kos.h>
#include <time.h>
#include <malloc.h>
#include <assert.h>
#include <unwind.h>
#include <pthread.h>

//C strings
#define sprintf_s snprintf

//Defines/undefs
extern unsigned long end;
extern unsigned long start;
#define _end end
#define _start start