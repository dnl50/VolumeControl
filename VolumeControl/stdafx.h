// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifndef SAFERELEASE

#define SAFERELEASE(ptr) \
	if(ptr == nullptr) { \
	} else { ptr->Release(); }	
#endif

#ifdef _DEBUG

#define DEBUG_PRINT(str) \
	std::cout << "DEBUG_PRINT: " << str << " File: " << __FILE__ << " | Line: " << __LINE__ << std::endl;

#define DEBUG_ERR_PRINT(str) \
	std::cout << "DEBUG_ERR_PRINT: Error in Line " << __LINE__ << " in File " << __FILE__ << ":" << str << std::endl;

#include <cassert>

#define ASSERT(expr) \
	assert(expr);

#else

#define DEBUG_PRINT(str)

#define DEBUG_ERR_PRINT(str)

#define ASSERT(expr)

#endif


// TODO: reference additional headers your program requires here
