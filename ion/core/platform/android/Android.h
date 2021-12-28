///////////////////////////////////////////////////
// File:		Switch.h
// Date:		2nd September 2019
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#pragma once

//Includes
#include <unistd.h>
#include <cstdio>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxxabi.h>
#include <string.h>

//C strings
#define sprintf_s snprintf