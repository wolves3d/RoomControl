#ifndef __pch_h_included__
#define __pch_h_included__

#define CLIENT_IMPL

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <strsafe.h>

// It should be mentioned that if you try to access COM ports > 9 with CreateFile,
// you'll always get ERROR_FILE_NOT_FOUND, even if the port exists.
// To avoid that behaviour, the port name should be passed as \\.\COMx (replacing x with the port number we want to test)
// Link: support.microsoft.com/kb/115831
#define ARDUINO_PORT "\\\\.\\COM18"

#else

#include <unistd.h>
#define ARDUINO_PORT "/dev/ttyUSB0"

#endif


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <string>  /* String function definitions */
#include <vector>
using namespace std;


typedef unsigned int uint;
typedef unsigned char byte;

struct System
{
	static void SleepMS(uint ms);
};

#endif // #ifndef __pch_h_included__
