#include "pch.h"


void System::SleepMS(uint ms)
{
	#ifdef WIN32
		Sleep(ms);
	#else
		usleep(1000 * ms);
	#endif
}