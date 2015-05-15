#include "NetListener.h"
#include "../NetPacket.h"


#include "MySqlClient.h"
#include "SensorManager.h"


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
		printf("server: get client GUID\n");
		return 0;
	}

	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		printf("server: response - get client GUID\n");
	}
};


class CServer : public INetListenerDelegate
{
	CNetListener m_netListener;
	CCommandManager m_cmdMgr;
	CMySqlClient m_dataBase;
	CSensorManager m_sensorManager;

	virtual void OnClientConnected(CTcpSocket * socket)
	{
		printf("accepted connection from %s, port %d\n",
			inet_ntoa(socket->m_addr.sin_addr),
			htons(socket->m_addr.sin_port));

		m_cmdMgr.GetPacketManager()->AddClent(socket);

		GetClientGUID cmd;
		m_cmdMgr.SendCommand(socket, &cmd);
	}
	

public:

	CServer(int port)
		: m_netListener(this)
		, m_cmdMgr(new CNetPacket)
	{
		m_netListener.Init(port);
		m_cmdMgr.RegisterHandler(new GetClientGUID());

		m_dataBase.Connect("192.168.0.1", "root", "trust#1sql", "smart_home");
		m_sensorManager.Init(&m_dataBase);
	}

	void OnUpdate()
	{
		m_sensorManager.UpdateSystemSensors();

		m_cmdMgr.OnUpdate();
		m_netListener.OnUpdate();
	}

	void OnShutdown()
	{
		m_netListener.Shutdown();
	}
};