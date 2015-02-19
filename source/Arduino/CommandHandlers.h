

#define HANDLER_HEADER(HANDLER_NAME, COMMAND_ID) \
	public: \
	virtual const char * GetName() const { return #HANDLER_NAME; } \
	virtual uint GetCommandID() const { return COMMAND_ID; } \
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)



class PingResponse : public IResponseHandler
{
	HANDLER_HEADER(PingResponse, RSP_PING)
	{
		int workTime = *((short *)data);
		printf("ping %d sec\n", workTime);
	}
};


class OneWireEnumBegin : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumBegin, RSP_ONE_WIRE_ENUM_BEGIN)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		device->ClearOneWireDeviceList();
		printf("1wire enum started!\n");
	}
};


class OneWireRomFound : public IResponseHandler
{
	HANDLER_HEADER(OneWireRomFound, RSP_ONE_WIRE_ROM_FOUND)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		/*
		byte addr[8];
		uint r = m_port.Recv(addr, 8);
		*/

		printf("RSP: 1wire rom found addr ");
		for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
			printf("%02X ", data[i]);
		printf("\n");



		device->AddOneWireDevice(OneWireAddr((const byte *)data));

		//g_commMgr->SendCommand(CMD_OW_READ_TEMP_SENSOR_DATA, addr, 8);
	}
};


class OneWireEnumEnd : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumEnd, RSP_ONE_WIRE_ENUM_END)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		device->OnOneWireEnumerated();
		printf("1wire enumerated!\n");
	}
};


class OneWireTemperature : public IResponseHandler
{
	HANDLER_HEADER(OneWireTemperature, RSP_OW_TEMP_SENSOR_DATA)
	{
		// hack objects
		OneWireAddr * addr = (OneWireAddr *)data;
		data = data + OneWireAddr::ADDR_LEN;

		/*
		printf("RSP: sensor data id:");
		for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
		printf("%02X ", dstBuffer[i]);

		printf("data: ");
		for (uint i = 0; i < OneWireAddr::DATA_LEN; ++i)
		printf("%02X ", dstBuffer[OneWireAddr::ADDR_LEN + i]);
		*/

		int raw = (data[1] << 8) | data[0];

		switch (addr->GetChipID())
		{
			case ECID_DS1822:
			case ECID_DS18B20:
			{
				byte cfg = (data[4] & 0x60);
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
				if (data[7] == 0x10)
				{
					// "count remain" gives full 12 bit resolution
					raw = (raw & 0xFFF0) + 12 - data[6];
				}
			}
			break;

			default:
				printf("UNKNOWN CHIP ID!");
		}

		float celsius = (float)raw / 16.f;
		printf("Temperature = %02.02fC / %02.02fF\n", celsius, (32 + (1.8f * celsius)));

		const string sensorID(addr->ToString());
		//LogDB(sensorID.c_str(), celsius);
	}
};


class ReadEEPROM : public IResponseHandler
{
	HANDLER_HEADER(ReadEEPROM, RSP_READ_EEPROM)
	{
		printf("RSP: EEPROM");

		for (uint i = 0; i < size; ++i)
		{
			printf(" %02X", data[i]);
		}

		printf("\n");
	}
};

class WriteEEPROM : public IResponseHandler
{
	HANDLER_HEADER(WriteEEPROM, RSP_WRITE_EEPROM)
	{
		printf("RSP: write EEPROM %d bytes OK\n", data[0]);
	}
};