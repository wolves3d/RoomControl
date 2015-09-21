#include "pch.h"
#include "Client.h"


CClient * g_client = NULL;
CCommandManager * g_arduinoCmdManager = NULL;


class OnClientGUIDReady : public INetCommand
{
	COMMAND_HEADER(OnClientGUIDReady, RSP_GET_CLIENT_GUID, CMD_NOP);
// 	{
// 		printf("server: response - get client GUID\n");
// 	}
};


struct ArduinoDeviceEnumeration : ICallback
{
	CCommandManager * m_commandManager;

	uint m_count;
	ArduinoDeviceEnumeration(uint count, CCommandManager * mgr)
		: m_count(count)
		, m_commandManager(mgr)
	{
	}

	virtual bool CallBack(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		// записываем результат во внутреннюю структуру
		--m_count;

		if (0 == m_count)
		{
			// Все ожидаемые устройства ответили

			OnEnumerationDone();
			// Убить себя!
			return true;
		}

		return false;
	}

	void OnEnumerationDone()
	{
		printf("client: all arduino devices enumerated!\n");

		OnClientGUIDReady cmd;
		CCommandManager * mgr = g_client->GetServer();
		mgr->SendCommand(g_client->GetServerSocket(), &cmd);
	}
};


class ClientGetGUIDResponse
	: public IResponseHandler
	, public ICallback
{
	struct HubDesc
	{
		uint expectedResponseCount;
		IAbstractSocket * requestingSocket;
		// GUID
	};

	HANDLER_HEADER(ClientGetGUIDResponse, CMD_GET_CLIENT_GUID)
	{
		printf("client: got request form server: get client GUID\n");

		vector <ArduinoDevice *> deviceList = g_arduinoManager->BuildDeviceList();
		const uint deviceCount = deviceList.size();

		printf("\tarduino device count: %d\n", deviceCount);

		if (0 == deviceCount)
		{
			//mgr->SendCommand(socket, RSP_GET_CLIENT_GUID, "NULL", 4);
			return;
		}

		printf("\t1-wire enumeration engaged\n");

		HubDesc * hubDesc = new HubDesc;
		hubDesc->expectedResponseCount = deviceCount;
		hubDesc->requestingSocket = socket; // FIXME: may die, need hard link with socket
		// Set timeOut for next commands here

		//ArduinoDeviceEnumeration * enumCallback = new ArduinoDeviceEnumeration(deviceCount, mgr);

		for (uint i = 0; i < deviceCount; ++i)
		{
			ArduinoDevice * device = deviceList[i];

			ArduinoReadEEPROM * command = new ArduinoReadEEPROM(0, 8);
			command->SetCallback(this, hubDesc);

			g_arduinoCmdManager->SendCommand(device->GetPort(), command);
		}
	}

	virtual bool CallBack(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr, void * userArg)
	{
		HubDesc * hubDesc = (HubDesc *)userArg;

		--(hubDesc->expectedResponseCount);

		//ArduinoDevice->SetGUID
		//Fill device desc (1-wire)

		if (0 == hubDesc->expectedResponseCount)
		{
			//CMD_GET_CLIENT_GUID
			//g_client->GetServer()->SendCommand()

			//mgr->SendCommand(socket, RSP_GET_CLIENT_GUID, "VOVA", 4);

			delete hubDesc;
			return true;
		}

		return false;
	}
};



CClient::CClient()
	: m_cmdManager(new CNetPacket())
	, m_arduinoCmdManager(new CSerialPacket)
{
	g_client = this;
	g_arduinoCmdManager = &m_arduinoCmdManager;

	// FIXME: memory leak, no free on destruct
	m_arduinoCmdManager.RegisterHandler(new OneWireEnumBegin());
	m_arduinoCmdManager.RegisterHandler(new OneWireRomFound());
	m_arduinoCmdManager.RegisterHandler(new OneWireEnumEnd());
	m_arduinoCmdManager.RegisterHandler(new OneWireTemperature());
	m_arduinoCmdManager.RegisterHandler(new PingResponse());
//	m_arduinoCmdManager.RegisterHandler(new ReadEEPROM());
	m_arduinoCmdManager.RegisterHandler(new WriteEEPROM());


	m_cmdManager.RegisterHandler(new ClientGetGUIDResponse());
	m_cmdManager.RegisterHandler(new OnSetPinValue());


	CSerialPort * arduinoPort = new CSerialPort;
	if (true == arduinoPort->Open(ARDUINO_PORT))
	{
		System::SleepMS(2000); // time to init microcontroller
		m_arduinoCmdManager.GetPacketManager()->AddClent(arduinoPort);

		ArduinoDevice * device = m_arduinoMgr.FindDevice(arduinoPort); // register device

		INetCommand * readROM = new ArduinoReadEEPROM(0, 8);
		readROM->SetCallback(SEL_Response(&CClient::OnArduinoUID), this, NULL);
		m_arduinoCmdManager.SendCommand(arduinoPort, readROM);

//		m_arduinoCmdManager.SendCommand()

		//m_arduinoCmdManager.SendCommand(arduinoPort, CMD_REQUEST_ONE_WIRE_ENUM, NULL, 0);
	}
}


bool CClient::Init(const char * addr, uint port)
{
	m_serverSocket.Connect(addr, port);
	m_cmdManager.GetPacketManager()->AddClent(&m_serverSocket);
	return true;
}


void CClient::OnUpdate()
{
	m_arduinoCmdManager.OnUpdate();
	m_cmdManager.OnUpdate();
}


void CClient::OnArduinoUID(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg)
{
	ArduinoDevice * dev = m_arduinoMgr.FindDevice(socket, true);
	if (NULL != dev)
	{
		dev->SetUID(data, size);

		SendClientInfo infoSender;

		m_cmdManager.SendCommand(
			&m_serverSocket,
			&infoSender);
	}
}