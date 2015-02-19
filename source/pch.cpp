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