


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

		//workTime = (GetTickCount() - ttt);

		LOG_INFO("ping %d sec", workTime);
	}
};


class OneWireEnumBegin : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumBegin, RSP_ONE_WIRE_ENUM_BEGIN)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		LOG_INFO_TAG(ARDUINO_TAG, "Request 1wire enumeration");
		device->ClearOneWireDeviceList();
	}
};


class OneWireRomFound : public IResponseHandler
{
	HANDLER_HEADER(OneWireRomFound, RSP_ONE_WIRE_ROM_FOUND)
	{
		ArduinoDevice * device = g_arduinoManager->FindDevice(socket, true);

		if (NULL == device)
			return;

		const string addrString = HexStringFromBytes(data, OneWireAddr::ADDR_LEN, true);
		LOG_INFO_TAG(ARDUINO_TAG, "\tResponse 1wire ROM found:%s", addrString.c_str());
		
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
		LOG_INFO_TAG(ARDUINO_TAG, "Response 1wire enumerated");
	}
};

class ArduinoInvalidRequest : public IResponseHandler
{
	HANDLER_HEADER(OneWireEnumEnd, RSP_INVALID_REQUEST)
	{
		LOG_ERROR("ardunio: invalid request!");
	}
};


class WriteEEPROM : public IResponseHandler
{
	HANDLER_HEADER(WriteEEPROM, RSP_WRITE_EEPROM)
	{
		LOG_INFO("RSP: write EEPROM %d bytes OK", data[0]);
	}
};
