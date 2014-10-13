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
};

#define CMD_SIZE_IN_BYTES 2

// -----------------------------------------------------------------------------

#ifndef CLIENT_IMPL

class SerialCommand
{
public:

	static void Send(uint16_t cmdID, byte * data, byte count)
	{
		Serial.write((byte *)&cmdID, CMD_SIZE_IN_BYTES);

		if (count > 0)
		{
			Serial.write(data, count);
		}
	}
};

#endif // #ifndef CLIENT_IMPL