#ifndef __pch_h_included__
#define __pch_h_included__


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#endif


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <string>  /* String function definitions */
#include <vector>
#include <map>
#include <queue>
using namespace std;


#include "CodeBase.h"
//typedef unsigned int uint;
//typedef unsigned char byte;

#define FAIL(X) assert(false)

#define SAFE_CAST(TYPE, OBJ) dynamic_cast <TYPE> (OBJ)

#endif // #ifndef __pch_h_included__
