//==============================================================================
#ifndef __Client_h_included__
#define __Client_h_included__
//==============================================================================

#include "CodeBase/network/transport/CommandManager.h"
#include "CodeBase/network/serial_socket/serialport.h"
#include "CodeBase/network/tcp_socket/tcp_socket.h"

#include "Arduino/ArduinoManager.h"
#include "Arduino/CommandHandlers.h"

#include "Server/Server.h"
#include "NetProtocol.h"

#include "Arduino/ArduinoRequests.h"

#include "ClientRequests.h"
#include "ClientResponses.h"

#include "pugixml.hpp"


#define CLIENT_CONFIG_FILE "client_config.xml"
#define CLIENT_CONFIG_ROOT "RoomControlClientConfig"
#define CONFIG_SERVER_ADDR "server_addr"
#define CONFIG_CLIENT_UID "client_uid"
#define CONFIG_CLIENT_EMPTY_UID "EMPTY"
#define CONFIG_SERVER_ENABLED "server_enabled"


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
	void LoadConfig();
	void SaveConfig();

	// Callbacks
	void OnArduinoUID(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg);
	void OnTempSensorUpdated(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg);

	uint m_nextUpdate;
	pugi::xml_document m_config;
};

extern CClient * g_client;
extern CCommandManager * g_arduinoCmdManager;

//==============================================================================
#endif // #ifndef __Client_h_included__
//==============================================================================