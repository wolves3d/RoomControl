#include "pch.h"
#include "Server.h"


CServer * g_server = NULL;


CServer::CServer(int port)
	: m_netListener(this)
	, m_cmdMgr(new CNetPacket)
{
	g_server = this;
	m_netListener.Init(port);
	m_cmdMgr.RegisterHandler(new GetClientGUID());
	m_cmdMgr.RegisterHandler(new OnClientInfo());

	m_dataBase.Connect("192.168.0.1", "root", "trust#1sql", "smart_home");
	m_sensorManager.Init(&m_dataBase);
	m_ruleManager.Init();
}





string HexStringFromBytes(const byte * buffer, size_t size)
{
	string resultString;

	if (size > 0)
	{
		const size_t memSize = (2 * size) + 1; // plus one for null term
		char * destBuffer = NEW char[memSize];

		char * ptr = destBuffer;
		
		for (size_t i = 0; i < size; ++i)
		{
			int byteValue = (char)buffer[i];
			sprintf(ptr, "%02X", byteValue);

			ptr += 2;
		}

		resultString = destBuffer;
		DEL_ARRAY(destBuffer);
	}

	return resultString;
}




void CServer::AddArduinoRecord(const byte * UID, size_t uidSize, IAbstractSocket * socket)
{
	string uidString = HexStringFromBytes(UID, uidSize);

	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(uidString);
	if (m_arduinoSockets.end() != it)
	{
		DEBUG_MSG("arduino UID duplicate");
	}

	m_arduinoSockets[uidString] = socket;
}


IAbstractSocket * CServer::GetArduinoSocket(const byte * UID, size_t uidSize)
{
	string uidString = HexStringFromBytes(UID, uidSize);
	
	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(uidString);
	if (m_arduinoSockets.end() != it)
	{
		return (it->second);
	}

	return NULL;
}