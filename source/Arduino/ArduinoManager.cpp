#include "pch.h"
#include "CommandManager.h"
#include "ArduinoManager.h"
#include "CommandHandlers.h"


ArduinoManager * g_arduinoManager = NULL;


ArduinoManager::ArduinoManager()
{
	if (NULL != g_arduinoManager)
	{
		FAIL("signletone reassign!");
	}

	g_arduinoManager = this;
}


ArduinoDevice * ArduinoManager::FindDevice(IAbstractSocket * socket, bool failOnNotFound)
{
	if (NULL == socket)
	{
		FAIL("Invalid socket!");
		return NULL;
	}

	DeviceMap::const_iterator it = m_deviceMap.find(socket);
	if (m_deviceMap.end() != it)
	{
		return (it->second);
	}
	else if (true == failOnNotFound)
	{
		FAIL("Arduino device not found!");
		return NULL;
	}

	ArduinoDevice * newDevice = new ArduinoDevice(socket);
	m_deviceMap[socket] = newDevice;

	return newDevice;
}


vector <ArduinoDevice *> ArduinoManager::BuildDeviceList()
{
	vector <ArduinoDevice *> result;

	DeviceMap::const_iterator it = m_deviceMap.begin();
	while (m_deviceMap.end() != it)
	{
		result.push_back(it->second);
		++it;
	}

	return result;
}



void ArduinoDevice::ClearOneWireDeviceList()
{
	m_isOneWireEnumerated = false;
	m_owDeviceList.clear();

	printf("RSP: 1wire enum begin\n");
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