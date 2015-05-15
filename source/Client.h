//==============================================================================
#ifndef __Client_h_included__
#define __Client_h_included__
//==============================================================================

#include "CommandManager.h"
#include "serialport.h"
#include "tcp_socket.h"

#include "Arduino/ArduinoManager.h"
#include "Arduino/CommandHandlers.h"

#include "Server/Server.h"


class CClient
{
	CCommandManager m_cmdManager;
	CTcpSocket m_serverSocket;
	
	ArduinoManager m_arduinoMgr;
	CCommandManager m_arduinoCmdManager;

public:

	CClient();
	bool Init(const char * addr, uint port);
	void OnUpdate();

	CCommandManager * GetServer() { return &m_cmdManager; }
	CTcpSocket * GetServerSocket() { return &m_serverSocket; }
};

extern CClient * g_client;
extern CCommandManager * g_arduinoCmdManager;

//==============================================================================
#endif // #ifndef __Client_h_included__
//==============================================================================