#include "pch.h"
#include "server.h"


void OnClientInfo::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	if (8 != size)
	{
		DEBUG_MSG("invalid buffer size!");
		return;
	}

	g_server->AddArduinoRecord(data, size, socket);
}