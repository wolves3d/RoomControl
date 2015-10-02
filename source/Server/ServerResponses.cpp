#include "pch.h"
#include "Server.h"


void OnClientInfo::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	if (8 != size)
	{
		DEBUG_MSG("invalid buffer size!");
		return;
	}

	g_server->AddArduinoRecord(data, size, socket);
}


void OnClientSensorUpdate::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	float sensorValue = *((float *)data);
	data += sizeof(float);

	byte pathLen = *((byte *)data);
	data += sizeof(byte);

	char sensorPath[0xFF];
	strncpy(sensorPath, (const char *)data, pathLen);
	sensorPath[pathLen] = 0; //FIXME

	const uint sensorID = g_sensorMgr->GetSensorIdByPath(sensorPath);
	CSensor *sensor = g_sensorMgr->GetSensor(sensorID);
	if (NULL != sensor)
	{
		g_sensorMgr->UpdateSensor(sensorID, sensorValue, false);
	}
}
