

REQUEST_HANDLER_DECL(ArduinoEnumerateOneWire, CMD_REQUEST_ONE_WIRE_ENUM, 0);


class ArduinoSetPinValue : public INetCommand
{
	REQUEST_HANDLER_BODY(ArduinoSetPinValue, CMD_PIN_WRITE, CMD_PIN_WRITE);

	ArduinoSetPinValue(byte pinPrefix, byte pin, byte pinValue)
		: m_pinPrefix(pinPrefix)
		, m_pin(pin)
		, m_pinValue(pinValue)
	{
	}

	virtual uint OnFillData(void * buffer, uint maxByteCount);
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr);

private:
	byte m_pinPrefix, m_pin, m_pinValue;
};


class ArduinoReadEEPROM : public INetCommand
{
	REQUEST_HANDLER_BODY(ArduinoReadEEPROM, CMD_READ_EEPROM, CMD_READ_EEPROM);

	ArduinoReadEEPROM(size_t offset, size_t byteCount)
		: m_offset(offset)
		, m_byteCount(byteCount)
	{
		if ((offset + byteCount) >= MAX_ROM_SIZE)
		{
			m_offset = 0;
			m_byteCount = 0;
			LOG_ERROR("Wrong params");
		}
	}

	virtual uint OnFillData(void * buffer, uint maxByteCount);
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr);
	

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


class ArduinoReadOneWire : public INetCommand
{
	REQUEST_HANDLER_BODY(ArduinoReadOneWire, CMD_OW_READ_TEMP_SENSOR_DATA, CMD_OW_READ_TEMP_SENSOR_DATA);
	
	ArduinoReadOneWire(const OneWireAddr &addr)
		: m_addr(addr.Address())
		, m_probeValue(0.0f)
	{
	}

	const OneWireAddr & GetOneWireAddr() const { return m_addr; }
	float GetProbeValue() const { return m_probeValue; }

	virtual uint OnFillData(void * buffer, uint maxByteCount);
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr);

private:
	OneWireAddr m_addr;
	float m_probeValue;
};