



class ArduinoSetPinValue : public INetCommand
{
	REQUEST_HANDLER_BODY(ArduinoSetPinValue, CMD_PIN_WRITE, NULL);

	ArduinoSetPinValue(byte pinPrefix, byte pin, byte pinValue)
		: m_pinPrefix(pinPrefix)
		, m_pin(pin)
		, m_pinValue(pinValue)
	{
	}

private:
	byte m_pinPrefix, m_pin, m_pinValue;
};


class ArduinoReadEEPROM : public INetCommand
{
	REQUEST_HANDLER_BODY(ArduinoReadEEPROM, CMD_READ_EEPROM, RSP_READ_EEPROM);
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr);
	//COMMAND_HEADER(ArduinoReadEEPROM, CMD_READ_EEPROM, RSP_READ_EEPROM);

	ArduinoReadEEPROM(size_t offset, size_t byteCount)
		: m_offset(offset)
		, m_byteCount(byteCount)
	{
		if ((offset + byteCount) >= MAX_ROM_SIZE)
		{
			m_offset = 0;
			m_byteCount = 0;
			DEBUG_MSG("Wrong params");
		}
	}

	

private:

	enum
	{
		MAX_ROM_SIZE = 128
	};

	// params
	size_t m_offset;
	size_t m_byteCount;

	// response data
	byte m_romData[MAX_ROM_SIZE];
};




