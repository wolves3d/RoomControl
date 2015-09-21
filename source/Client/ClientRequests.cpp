#include "pch.h"
#include "Client.h"


uint SendClientInfo::OnFillData(void * buffer, uint maxByteCount)
{
	const uint bytesWritten = ArduinoDevice::UID_SIZE;
	if (bytesWritten > maxByteCount)
	{
		DEBUG_MSG("Target buffer too small");
		return 0;
	}

	ArduinoVector deviceList = g_arduinoManager->BuildDeviceList();
	ArduinoDevice * dev = deviceList[0];

	const byte * UID = dev->GetUID();
	memcpy(buffer, UID, bytesWritten);


	return bytesWritten;
}