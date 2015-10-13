#include "pch.h"


void System::SleepMS(uint ms)
{
	#ifdef WIN32
		Sleep(ms);
	#else
		usleep(1000 * ms);
	#endif
}


string u_string_format(const char *fmt, ...)
{
	char ret[4 * 4096 + 1] = { 0 };
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(ret, 4 * 4096, fmt, ap);
	va_end(ap);

	std::string str(ret);

	return str;
}


string HexStringFromBytes(const byte * buffer, size_t size, bool spaceSeparator)
{
	string resultString;

	if (size > 0)
	{
		const uint charPerByte = (false == spaceSeparator)
			? 2
			: 3;

		const char *fmt = (false == spaceSeparator)
			? "%02X"
			: "%02X ";

		const size_t memSize = (charPerByte * size) + 1; // plus one for null term
		char * destBuffer = NEW char[memSize];

		char * ptr = destBuffer;

		for (size_t i = 0; i < size; ++i)
		{
			unsigned int byteValue = (byte)buffer[i];

			sprintf(ptr, fmt, byteValue);

			ptr += charPerByte;
		}

		resultString = destBuffer;
		DEL_ARRAY(destBuffer);
	}

	return resultString;
}


void AssertMsg(const char * szFile, int nLine, const char * szString)
{
	#ifdef WIN32
	
	string messageString = u_string_format(
		"Assertion Failed : %s\nFile: \"%s\" Line: %d\nYes - break into debugger, No - Exit program, Cancel - skip",
		szString, szFile, nLine);


	int nResult = MessageBox(NULL, messageString.c_str(), "Assertion Failed", MB_ICONSTOP | MB_YESNOCANCEL);

	switch (nResult)
	{
	case IDYES:
		//SysWrite("User decided to break into debuger.\n");
		__debugbreak(); //__asm { int 3 }
		return;

	case IDNO:
		//SysWrite("User decided close application.");
		exit(-1);
		return;
	}
	
	#endif // #ifdef WIN32
}
