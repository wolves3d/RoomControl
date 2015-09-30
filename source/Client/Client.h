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
#include "NetProtocol.h"

#include "Arduino/ArduinoRequests.h"

#include "ClientRequests.h"
#include "ClientResponses.h"


class CClient : public CBaseObject
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

private:

	void OnEverySecondUpdate();

	// Callbacks
	void OnArduinoUID(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg);
	void OnTempSensorUpdated(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg);

	uint m_nextUpdate;
};

extern CClient * g_client;
extern CCommandManager * g_arduinoCmdManager;

//==============================================================================
#endif // #ifndef __Client_h_included__
//==============================================================================