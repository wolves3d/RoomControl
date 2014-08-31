enum ECommandID
{
	CMD_PIN_WRITE,
	CMD_PIN_READ,

	CMD_PIN_WATCH,	// Отслеживать изменение значения pin
	CMD_PIN_STOP_WATCH,	// Прекратить отслеживать значения pin
	RSP_ON_PIN_CHANGED,	// Ответ при определении изменения

	CMD_REQUEST_ONE_WIRE_ENUM,
	CMD_ONE_WIRE_ENUM_BEGIN,
	CMD_ONE_WIRE_ROM_FOUND,
	CMD_ONE_WIRE_ENUM_END,

	CMD_INVALID_REQUEST,
};

class SerialCommand
{
public:

	static void Send(uint16_t cmdID, byte * data, byte count)
	{
		Serial.write((byte *)&cmdID, 2);

		if (count > 0)
		{
			Serial.write(data, count);
		}
	}
};