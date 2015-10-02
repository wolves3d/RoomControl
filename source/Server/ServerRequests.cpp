#include "pch.h"
#include "Server.h"


uint SetPinValue::OnFillData(void * buffer, uint maxByteCount)
{
	const char * pinString = m_pinName.c_str();

	byte pinPrefix = ('A' == pinString[0])
		? 'A'
		: 'D';

	byte pinNumber = atoi(pinString + 1);

	byte *destBuffer = (byte *)buffer;

	destBuffer[0] = pinPrefix;
	destBuffer[1] = pinNumber;
	destBuffer[2] = m_pinValue;
	return 3;
}
