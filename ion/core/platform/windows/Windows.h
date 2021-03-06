///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#pragma once

//Includes
#include <Windows.h>
#include <direct.h>
#include <shlobj.h>
#include <Dbghelp.h>

//Defines/undefs
#ifdef DELETE
#undef DELETE
#endif

#ifdef MessageBox
#undef MessageBox
#endif

#ifdef Yield
#undef Yield
#endif

#ifdef DialogBox
#undef DialogBox
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

#ifdef CreateDirectory
#undef CreateDirectory

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#endif