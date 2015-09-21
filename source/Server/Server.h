#include "NetListener.h"
#include "../NetPacket.h"
#include "NetProtocol/NetProtocol.h"
//#include "Arduino/CommandHandlers.h"


#include "MySqlClient.h"
#include "SensorManager.h"
#include "RuleManager.h"

#include "ServerRequests.h"
#include "ServerResponses.h"


#define SERVER_PORT 35999


#define COMMAND_HEADER(COMMAND_NAME, COMMAND_ID, RESPONSE_ID) \
	public: \
	virtual const char * GetName() const { return #COMMAND_NAME; } \
	virtual uint GetCommandID() const { return COMMAND_ID; } \
	virtual uint GetResponseID() const { return RESPONSE_ID; }


class GetClientGUID : public INetCommand
{
	COMMAND_HEADER(GetClientGUID, CMD_GET_CLIENT_GUID, RSP_GET_CLIENT_GUID);

	virtual uint OnFillData(void * buffer, uint maxByteCount)
	{
		printf("server: request client GUID\n");
		return 0;
	}

	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		printf("server: response - got client GUID\n");
	}
};


class CServer : public INetListenerDelegate
{
	CNetListener m_netListener;
	CCommandManager m_cmdMgr;
	CMySqlClient m_dataBase;
	CSensorManager m_sensorManager;
	CRuleManager m_ruleManager;

	virtual void OnClientConnected(CTcpSocket * socket)
	{
		printf("accepted connection from %s, port %d\n",
			inet_ntoa(socket->m_addr.sin_addr),
			htons(socket->m_addr.sin_port));

		m_cmdMgr.GetPacketManager()->AddClent(socket);

//		GetClientGUID cmd;
//		m_cmdMgr.SendCommand(socket, &cmd);
	}
	

public:

	CServer(int port);

	void OnUpdate()
	{
		m_sensorManager.UpdateSystemSensors();
		m_ruleManager.OnUpdate();

		m_cmdMgr.OnUpdate();
		m_netListener.OnUpdate();
	}

	CMySqlClient * GetDB() { return &m_dataBase; }
	CCommandManager * GetCommandManager() { return &m_cmdMgr; }

	void OnShutdown()
	{
		m_netListener.Shutdown();
	}

	void AddArduinoRecord(const byte * UID, size_t uidSize, IAbstractSocket * socket);
	IAbstractSocket * GetArduinoSocket(const byte * UID, size_t uidSize);

private:

	typedef map <string, IAbstractSocket *> ArduinoSocketMap;
	ArduinoSocketMap m_arduinoSockets;
};


extern CServer * g_server;