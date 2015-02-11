

class CommandManager
{
#define MAX_PACK_SIZE 16
	byte m_buffer[MAX_PACK_SIZE];
	byte m_writeOffset;
	byte m_readOffset;
	byte m_size;
	byte m_targetSize;

	byte m_cmdID;
	bool m_waitingForData;


public:
	CommandManager()
		: m_writeOffset(0)
		, m_readOffset(0)
		, m_size(0)
		, m_targetSize(2)
		, m_waitingForData(true)
	{
	}

	bool PinWrite()
	{
		byte pinID = PopByte();
		byte pinValue = PopByte();

		pinMode(pinID, OUTPUT);
		digitalWrite(pinID, pinValue);

		return true;
	}

	bool OneWireReadTempData()
	{
		const byte buffSize = OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN;
		byte resBuffer[buffSize];
		memset(resBuffer, 0, buffSize);


		// 		if (false == WaitForData(resBuffer, OneWireAddr::ADDR_LEN))
		// 			return false;

		// hack objects
		//OneWireAddr * blob = (OneWireAddr *)resBuffer;
		byte * sensorData = resBuffer + OneWireAddr::ADDR_LEN;

		for (byte i = 0; i < OneWireAddr::ADDR_LEN; ++i)
		{
			resBuffer[i] = PopByte();
		}

		if (false == oneWire.ReadTemperatureData(sensorData, resBuffer, OneWireAddr::DATA_LEN))
			return false;

		SerialCommand::Send(RSP_OW_TEMP_SENSOR_DATA, resBuffer, (OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN));
		return true;
	}

	bool BoundCheckEEPROM(byte offset, byte count)
	{
		if (count < 9)
		{
			if (offset < 246)
			{
				return true;
			}
		}

		return false;
	}

	bool ReadEEPROM()
	{
		//if (2 != ByteCount())
		{
			InvalidateBytes();
			return false;
		}

		byte offset = PopByte();
		byte count = PopByte();

		if (false == BoundCheckEEPROM(offset, count))
		{
			return false;
		}

		byte resBuffer[8];

		for (byte i = 0; i < count; ++i)
		{
			resBuffer[i] = EEPROM.read(offset + i);
		}
		
		SerialCommand::Send(RSP_READ_EEPROM, resBuffer, count);
		return true;
	}

	bool WriteEEPROM()
	{
		if (ByteCount() > 2)
		{
			byte offset = PopByte();
			byte count = PopByte();

			if (true == BoundCheckEEPROM(offset, count))
			{
				if (count == ByteCount())
				{
					for (byte i = 0; i < count; ++i)
					{
						EEPROM.write(offset + i, PopByte());
					}

					SerialCommand::Send(RSP_WRITE_EEPROM, &count, 1);
					return true;
				}
			}
		}

		InvalidateBytes();
		return false;
	}

	void OnCommand(byte cmdID)
	{
		bool succeedFlag = false;

		switch (cmdID)
		{
		case CMD_PIN_WRITE: succeedFlag = PinWrite(); break;
		case CMD_REQUEST_ONE_WIRE_ENUM: succeedFlag = oneWire.Enumerate(); break;
		case CMD_OW_READ_TEMP_SENSOR_DATA: succeedFlag = OneWireReadTempData(); break;

		case CMD_READ_EEPROM: succeedFlag = ReadEEPROM(); break;
		case CMD_WRITE_EEPROM: succeedFlag = WriteEEPROM(); break;

		default:
			SerialCommand::Send(RSP_INVALID_CMD, (byte *)&cmdID, 2);
			return;
		}

		if (false == succeedFlag)
		{
			SerialCommand::Send(RSP_INVALID_REQUEST, (byte *)&cmdID, 2);
		}
	}

	byte ByteCount()
	{
		return m_size;
	}

	void InvalidateBytes()
	{
		m_size = 0;
	}

	void PushByte(byte value)
	{
		m_buffer[m_size] = value;
		++m_size;
	}

	byte PopByte()
	{
		byte result = 0xFF;

		if (m_size > 0)
		{
			result = m_buffer[0];
			--m_size;

			// move rest
			for (byte i = 0; i < m_size; ++i)
			{
				m_buffer[i] = m_buffer[i + 1];
			}
		}


		return result;
	}


	void OnLoop()
	{
		if (true == Serial.available())
		{
			PushByte(Serial.read());
		}

		if (ByteCount() >= m_targetSize)
		{
			if (true == m_waitingForData)
			{
				// command recieved
				m_cmdID = PopByte();

				// command argument size
				m_targetSize = PopByte();
				m_waitingForData = false;
			}
			else
			{
				// ready to call command
				OnCommand(m_cmdID);

				// prepare for next
				m_waitingForData = true;
				m_targetSize = 2;
			}
		}

		// ------

		if (true == oneWire.IsEnumerating())
		{
			OneWireAddr ow_blob;
			oneWire.GetNextAddr(&ow_blob);
		}
	}
};