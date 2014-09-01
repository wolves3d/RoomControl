enum EOneWireChipID
{
	ECID_DS18S20	= 0x10, // or old DS1820
	ECID_DS1822		= 0x22,
	ECID_DS18B20	= 0x28,
};

struct OneWireAddr
{
#define OW_ADDR_LEN 8
	byte addr[OW_ADDR_LEN];

	OneWireAddr()
	{
		Set(NULL);
	}

	void Set(OneWireAddr * other)
	{
		if (NULL == other)
		{
			memset(addr, 0x0, OW_ADDR_LEN);
		}
		else
		{
			memcpy(addr, other->Address(), OW_ADDR_LEN);
		}
	}

	uint8_t * Address()
	{
		return addr;
	}

	byte GetChipID()
	{
		return addr[0];
	}

	bool CheckCRC()
	{
		// last byte is CRC sum of first 7
		return (OneWire::crc8(addr, 7) == addr[7]);
	}

	bool IsEqual(const OneWireAddr & other)
	{
		for (byte i = 0; i < OW_ADDR_LEN; ++i)
		{
			if (addr[i] != other.addr[i])
				return false;
		}

		return true;
		//return (0 == memcmp(addr, other.addr, OW_ADDR_LEN));
	}

	void DebugPrintAddress()
	{
		for (byte i = 0; i < OW_ADDR_LEN; ++i)
		{
			Serial.print(addr[i], HEX);
			Serial.write(' ');
		}
	}
};


class OneWireWrapper : public OneWire
{
private:
	bool m_enumMode;
	byte m_blobCount;
	OneWireAddr m_selectedBlob;

public:
	OneWireWrapper(uint8_t pin)
		: OneWire(pin)
		, m_enumMode(false)
		, m_blobCount(0)
	{
	}

	bool Enumerate()
	{
		if (true == m_enumMode)
			return false; // enum in progress

		m_selectedBlob.Set(NULL);
		m_enumMode = true;
		m_blobCount = 0;
		return true;
	}


	bool GetNextAddr(OneWireAddr * outBlob)
	{
		if (true == m_enumMode)
		{
			if (0 == m_blobCount)
			{
				SerialCommand::Send(RSP_ONE_WIRE_ENUM_BEGIN, 0, 0);
			}

			if (0 != search(outBlob->Address()))
			{
				// found
				++m_blobCount;
				SerialCommand::Send(RSP_ONE_WIRE_ROM_FOUND, outBlob->Address(), 8);
				return true;
			}

			// search done
			reset_search();
			m_enumMode = false;
			SerialCommand::Send(RSP_ONE_WIRE_ENUM_END, 0, 0);
		}

		return false;
	}

	bool SelectBlob(OneWireAddr & blob)
	{
		if (false == m_selectedBlob.IsEqual(blob))
		{
			reset();
			select(m_selectedBlob.Address());
			m_selectedBlob.Set(&blob);
		}

		return true;
	}

	bool Write(OneWireAddr & blob, byte * data, byte count)
	{
		if (true == SelectBlob(blob))
		{
			for (byte i = 0; i < count; ++i)
			{
				write(data[i]);
			}

			return true;
		}

		return false;
	}

	#define MAX_BYTE_COUNT 9
	bool ReadTemperatureData(byte * buffer, OneWireAddr & blob, byte maxSize = MAX_BYTE_COUNT)
	{
		if (maxSize >= MAX_BYTE_COUNT)
		{
			reset();
			select(blob.Address());
			write(0x44, 1); // start conversion, with parasite power on at the end
			//  delay(750);     // maybe 750ms is enough, maybe not

			// we might do a ds.depower() here, but the reset will take care of it.
			//ds.depower();

			byte present = reset();
			select(blob.Address());
			write(0xBE);

			memset(buffer, 0, maxSize);
			read_bytes(buffer, MAX_BYTE_COUNT);
		}
	}
};