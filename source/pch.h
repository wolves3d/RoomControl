#ifndef __pch_h_included__
#define __pch_h_included__

#define CLIENT_IMPL

#ifdef WIN32

// It should be mentioned that if you try to access COM ports > 9 with CreateFile,
// you'll always get ERROR_FILE_NOT_FOUND, even if the port exists.
// To avoid that behaviour, the port name should be passed as \\.\COMx (replacing x with the port number we want to test)
// Link: support.microsoft.com/kb/115831
#define ARDUINO_PORT "\\\\.\\COM7"

#else

#define ARDUINO_PORT "/dev/ttyUSB0"

#endif


#include "CodeBase/CodeBase.h"

struct System
{
	static void SleepMS(uint ms);
};



string u_string_format(const char *fmt, ...);
string HexStringFromBytes(const byte * buffer, size_t size);

#endif // #ifndef __pch_h_included__
