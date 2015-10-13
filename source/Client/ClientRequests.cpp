#include "pch.h"
#include "Client.h"


uint SendClientInfo::OnFillData(void * buffer, uint maxByteCount)
{
	const uint bytesWritten = ArduinoDevice::UID_SIZE;
	if (bytesWritten > maxByteCount)
	{
		LOG_ERROR("Target buffer too small");
		return 0;
	}

	ArduinoVector deviceList = g_arduinoManager->BuildDeviceList();
	ArduinoDevice * dev = deviceList[0];

	const byte * UID = dev->GetUID();
	memcpy(buffer, UID, bytesWritten);


	return bytesWritten;
}


uint SendSensorData::OnFillData(void * buffer, uint maxByteCount)
{
	DEBUG_ASSERT(m_sensorPath.length() < 0xFF);
	const byte pathLen = m_sensorPath.length(); // FIXME 256 byte limit

	size_t offset = 0;
	
	byte *data = (byte *)buffer;

	float *valuePtr = (float *)data;
	(*valuePtr) = m_value;
	offset += sizeof(float);

	byte *stringLenPtr = (byte *)(data + offset);
	(*stringLenPtr) = pathLen;
	offset += sizeof(byte);

	memcpy((data + offset), m_sensorPath.c_str(), pathLen);
	offset += (*stringLenPtr);
	return offset;
}