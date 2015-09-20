//==============================================================================
#ifndef __netCommand_h_included__
#define __netCommand_h_included__
//==============================================================================

// Config
#define ANALOG_PIN_COUNT 8
#define ONE_WIRE_SIGNAL_PIN 10 // (a 4.7K resistor is necessary)

enum ECommandID
{
	CMD_NOP = 0,
	CMD_PIN_WRITE = 1,
	CMD_PIN_READ = 2,

	CMD_PIN_WATCH = 3,		// Отслеживать изменение значения pin
	CMD_PIN_STOP_WATCH = 4,	// Прекратить отслеживать значения pin
	RSP_ON_PIN_CHANGED = 5,	// Ответ при определении изменения

	CMD_REQUEST_ONE_WIRE_ENUM = 6,
	RSP_ONE_WIRE_ENUM_BEGIN = 7,
	RSP_ONE_WIRE_ROM_FOUND = 8,
	RSP_ONE_WIRE_ENUM_END = 9,

	CMD_OW_READ_TEMP_SENSOR_DATA = 10,
	RSP_OW_TEMP_SENSOR_DATA = 11,

	RSP_PING = 12,
	RSP_INVALID_REQUEST = 13,
	RSP_INVALID_CMD = 14,

	CMD_READ_EEPROM = 15,
	RSP_READ_EEPROM = 16,

	CMD_WRITE_EEPROM = 17,
	RSP_WRITE_EEPROM = 18,

	CMD_TABLE_SIZE	// must be at end of enum
};

// #define CMD_SIZE_IN_BYTES 2

// -----------------------------------------------------------------------------

#ifdef CLIENT_IMPL


class CSerialPacket : public ITransportPacket
{
	struct PacketHeader
	{
		byte	cmdID;			// Уникальный идентификатор команды
		byte	cmdTag;
		byte	dataByteCount;	// Размер аргумента команды
	};

public:

	// ITransportPacket --------------------------------------------------------

	virtual uint GetHeaderSize() const { return sizeof(PacketHeader); }
	virtual bool CheckHeader(const void * header, uint byteCount) { return false; }

	virtual uint GetArgumentSize(const void * packetBytes)
	{
		return (uint)((PacketHeader *)packetBytes)->dataByteCount;
	}

	virtual bool CheckPacket(const void * packet, uint byteCount) { return false; }

	virtual void FillHeader(void * dst, uint cmdID, uint cmdTag, uint byteCount)
	{
		if (NULL == dst)
			return;

		PacketHeader * header = (PacketHeader *)dst;

		header->cmdID = cmdID;
		header->cmdTag = cmdTag;
		header->dataByteCount = byteCount;

		// FIXME: add CRC checksums here!
	}

	virtual uint GetCommandID(const void * packetBytes)
	{
		return ((PacketHeader *)packetBytes)->cmdID;
	}

	virtual uint GetCommandTag(const void * packetBytes)
	{
		return ((PacketHeader *)packetBytes)->cmdTag;
	}
};

#else

class SerialCommand
{
public:

	static void Send(uint16_t cmdID, byte cmdTag, byte * data, byte count)
	{
		Serial.write((byte *)&cmdID, 1);
		Serial.write((byte *)&cmdTag, 1);
		Serial.write((byte *)&count, 1);

		if (count > 0)
		{
			Serial.write(data, count);
		}
	}
};

#endif // #ifndef CLIENT_IMPL

//==============================================================================
#endif // #ifndef __netCommand_h_included__
//==============================================================================