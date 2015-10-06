


class SendClientInfo : public INetCommand
{
	REQUEST_HANDLER_BODY(SendClientInfo, SEND_CLIENT_INFO, 0);
	virtual uint OnFillData(void * buffer, uint maxByteCount);
};


class SendSensorData : public INetCommand
{
	REQUEST_HANDLER_BODY(SendClientInfo, UPDATE_SENSOR_VALUE, 0);

	SendSensorData(const char * sensorPath, float sensorValue)
		: m_sensorPath(sensorPath)
		, m_value(sensorValue)
	{
	}

	virtual uint OnFillData(void * buffer, uint maxByteCount);

	string m_sensorPath;
	float m_value;
};