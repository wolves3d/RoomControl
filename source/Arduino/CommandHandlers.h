


static size_t ttt = 0;


class PingResponse : public IResponseHandler
{
	HANDLER_HEADER(PingResponse, RSP_PING)
	{

		if (0 == ttt)
		{
			ttt = GetTickCount();
		}

		DEBUG_ASSERT(2 == size);
		int workTime = *((short *)data);

		workTime = (GetTickCount() - ttt);

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

		printf("RSP: 1wire rom found addr ");
		for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
			printf("%02X ", data[i]);
		printf("\n");

		device->AddOneWireDevice(OneWireAddr((const byte *)data));
	}
};


class OneWireEnumEnd : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumEnd, CMD_REQUEST_ONE_WIRE_ENUM)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		device->OnOneWireEnumerated();
		printf("1wire enumerated!\n");
	}
};

class ArduinoInvalidRequest : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumEnd, RSP_INVALID_REQUEST)
	{
		printf("ardunio: invalid request!\n");
	}
};


class WriteEEPROM : public IResponseHandler
{
	HANDLER_HEADER(WriteEEPROM, RSP_WRITE_EEPROM)
	{
		printf("RSP: write EEPROM %d bytes OK\n", data[0]);
	}
};