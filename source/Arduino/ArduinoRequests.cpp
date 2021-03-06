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


void ArduinoSetPinValue::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	printf("RSP: SetPinValue\n");
}


uint ArduinoReadEEPROM::OnFillData(void * buffer, uint maxByteCount)
{
	printf("REQUEST: EEPROM OnFillData\n");

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


uint ArduinoReadOneWire::OnFillData(void * buffer, uint maxByteCount)
{
	memcpy(buffer, m_addr.Address(), m_addr.ADDR_LEN);
	return m_addr.ADDR_LEN;
}


void ArduinoReadOneWire::OnResponse(const byte * dat, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	DEBUG_ASSERT(size == (OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN));

	// hack object
	OneWireAddr addr(dat);
	const byte * oneWireBin = dat + OneWireAddr::ADDR_LEN;
	/*
	printf("ArduinoReadOneWire: sensor id:");
	for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
		printf("%02X ", dat[i]);

	printf("data: ");
	for (uint i = 0; i < OneWireAddr::DATA_LEN; ++i)
		printf("%02X ", oneWireBin[i]);
	
	*/
		int raw = (oneWireBin[1] << 8) | oneWireBin[0];

	switch (addr.GetChipID())
	{
		case ECID_DS1822:
		case ECID_DS18B20:
		{
			 byte cfg = (oneWireBin[4] & 0x60);
			// at lower res, the low bits are undefined, so let's zero them
			if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
			else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
			else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
			//// default is 12 bit resolution, 750 ms conversion time
		}
		break;

		case ECID_DS18S20:
		{
			raw = raw << 3; // 9 bit resolution default
			if (oneWireBin[7] == 0x10)
			{
				// "count remain" gives full 12 bit resolution
				raw = (raw & 0xFFF0) + 12 - oneWireBin[6];
			}
		}
		break;

		default:
		printf("UNKNOWN CHIP ID!");
	}

	float celsius = (float)raw / 16.f;
	printf("Temperature = %02.02fC / %02.02fF\n", celsius, (32 + (1.8f * celsius)));

	m_probeValue = celsius;
}