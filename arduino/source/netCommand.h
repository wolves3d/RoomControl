enum ECommandID
{
	CMD_PIN_WRITE,
	CMD_PIN_READ,

	CMD_PIN_WATCH,		// Отслеживать изменение значения pin
	CMD_PIN_STOP_WATCH,	// Прекратить отслеживать значения pin
	RSP_ON_PIN_CHANGED,	// Ответ при определении изменения

	CMD_REQUEST_ONE_WIRE_ENUM,
	RSP_ONE_WIRE_ENUM_BEGIN,
	RSP_ONE_WIRE_ROM_FOUND,
	RSP_ONE_WIRE_ENUM_END,

	CMD_OW_READ_TEMP_SENSOR_DATA,
	RSP_OW_TEMP_SENSOR_DATA,

	RSP_INVALID_REQUEST,
};

#define CMD_SIZE_IN_BYTES 2

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