#include "pch.h"
#include "Client/Client.h"


uint ArduinoSetPinValue::OnFillData(void * buffer, uint maxByteCount)
{
	byte *destBuffer = (byte *)buffer;

	destBuffer[0] = m_pinPrefix;
	destBuffer[1] = m_pin;
	destBuffer[2] = m_pinValue;

	DEBUG_ASSERT('D' == m_pinPrefix); // Analog not supported

	return 3;
}


uint ArduinoReadEEPROM::OnFillData(void * buffer, uint maxByteCount)
{
	byte * buf = (byte *)buffer;

	buf[0] = m_offset;
	buf[1] = m_byteCount;

	return 2;
}


void ArduinoReadEEPROM::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	DEBUG_ASSERT(m_byteCount == size);

	printf("RSP: EEPROM");

	for (uint i = 0; i < size; ++i)
	{
		m_romData[i] = data[i];
		printf(" %02X", data[i]);
	}

	printf("\n");
}