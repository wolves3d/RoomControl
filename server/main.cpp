#include "pch.h"
#include "ServerManager.h"


#define SERVER_PORT 35999
CCommandManager * g_cmdManager = NULL;



class PingCommand : public CNetworkCommand
{
	virtual NetCommandID UniqueID() const
	{
		return "ping";
	}

	virtual uint OnSend(BufferObject * dstData, CTcpSocket * dstClient)
	{
		uint offset = CNetworkCommand::OnSend(dstData, dstClient);
		offset = dstData->Write(offset, "PING", 5);

		return offset;
	}

	virtual void OnAnswer(CTcpSocket * srcClient, BufferObject * data)
	{
		bool test = data->Compare(0, "PONG", 5);
		test = false;
	}
};


struct MyDelegates : public IServerManagerDelegate
{
	virtual void OnClientConnected(CTcpSocket * socket)
	{
		printf("accepted connection from %s, port %d\n",
			inet_ntoa(socket->m_addr.sin_addr),
			htons(socket->m_addr.sin_port));

		/*
		CNetworkCommand
			id,
			args


		g_cmdManager->SendCommand(socket, );
		*/
	}
};

void main()
{
	MyDelegates d;
	CServerManager serverMgr(&d);

	g_cmdManager = serverMgr.GetCommandManager();
	//g_cmdManager->RegisterCommand(NULL /*handshake*/);
	//g_cmdManager->RegisterCommand(NULL /*goodbye*/);
	//g_cmdManager->RegisterCommand(NULL /*askClientSmth*/);
	g_cmdManager->RegisterCommand(new PingCommand());

	serverMgr.Init(SERVER_PORT);

	while (true == serverMgr.OnUpdate())
	{
		// sleep some?
	}

	serverMgr.Shutdown();
}