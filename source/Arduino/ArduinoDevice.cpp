#include "pch.h"
#include "CommandManager.h"
#include "ArduinoManager.h"


ArduinoDevice::ArduinoDevice(IAbstractSocket * socket)
	: m_comPort(socket)
{
	memset(m_UID, 0, UID_SIZE);
}


void ArduinoDevice::ClearOneWireDeviceList()
{
	m_isOneWireEnumerated = false;
	m_owDeviceList.clear();

	printf("ClearOneWireDeviceList\n");
}


void ArduinoDevice::AddOneWireDevice(OneWireAddr deviceAddr)
{
	m_owDeviceList.push_back(deviceAddr);
}


const OneWireAddr & ArduinoDevice::GetOneWireDevice(uint device)
{
	return m_owDeviceList[device];
}


void ArduinoDevice::OnOneWireEnumerated()
{
	m_isOneWireEnumerated = true;
	printf("RSP: 1wire enumerated!\n");
}


void ArduinoDevice::SetUID(const byte *data, uint dataSize)
{
	if (UID_SIZE == dataSize)
	{
		memcpy(m_UID, data, dataSize);
	}
	else
	{
		DEBUG_MSG("invalid UID data");
	}
}


const byte * ArduinoDevice::GetUID() const
{
	return m_UID;
}