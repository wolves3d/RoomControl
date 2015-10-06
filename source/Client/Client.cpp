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
		: m_commandManager(mgr)
		, m_count(count)
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

// 		OnClientGUIDReady cmd;
// 		CCommandManager * mgr = g_client->GetServer();
// 		mgr->SendCommand(g_client->GetServerSocket(), &cmd);
	}
};


CClient::CClient()
	: m_cmdManager(new CNetPacket(), false)
	, m_arduinoCmdManager(new CSerialPacket, true)
	, m_nextUpdate(0)
{
	g_client = this;
	g_arduinoCmdManager = &m_arduinoCmdManager;

	// FIXME: memory leak, no free on destruct
	m_arduinoCmdManager.RegisterHandler(new ArduinoInvalidRequest());
	m_arduinoCmdManager.RegisterHandler(new OneWireEnumBegin());
	m_arduinoCmdManager.RegisterHandler(new OneWireRomFound());
	m_arduinoCmdManager.RegisterHandler(new OneWireEnumEnd());
	m_arduinoCmdManager.RegisterHandler(new PingResponse());
	m_arduinoCmdManager.RegisterHandler(new WriteEEPROM());


	m_cmdManager.RegisterHandler(new OnSetPinValue());


	CSerialPort * arduinoPort = new CSerialPort;
	if (true == arduinoPort->Open(ARDUINO_PORT))
	{
		System::SleepMS(2000); // time to init microcontroller
		m_arduinoCmdManager.GetPacketManager()->AddClent(arduinoPort);

		/*ArduinoDevice * device = */m_arduinoMgr.FindDevice(arduinoPort); // register device


		INetCommand * readROM = new ArduinoReadEEPROM(0, 8);
		readROM->SetCallback(SEL_Response(&CClient::OnArduinoUID), this, NULL);
		m_arduinoCmdManager.SendCommand(arduinoPort, readROM);

		
		m_arduinoCmdManager.SendCommand(arduinoPort, new ArduinoEnumerateOneWire);
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

	if (GetTickCount() >= m_nextUpdate)
	{
		OnEverySecondUpdate();
	}
}


void CClient::OnEverySecondUpdate()
{
	m_nextUpdate = GetTickCount() + 1000;

	//return;
	printf("OnEverySecondUpdate\n");

	ArduinoVector deviceList = m_arduinoMgr.BuildDeviceList();
	for (uint i = 0; i < deviceList.size(); ++i)
	{
		ArduinoDevice * arduino = deviceList[i];
		for (uint k = 0; k < arduino->GetOneWireDeviceCount(); ++k)
		{
			const OneWireAddr &addr = arduino->GetOneWireDevice(k);

			ArduinoReadOneWire *tempReader = NEW ArduinoReadOneWire(addr);
			tempReader->SetCallback(SEL_Response(&CClient::OnTempSensorUpdated), this, tempReader);
			m_arduinoCmdManager.SendCommand(arduino->GetPort(), tempReader);
		}
	}
}


void CClient::OnTempSensorUpdated(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg)
{
	ArduinoReadOneWire *tempReader = (ArduinoReadOneWire *)userArg;
	ArduinoDevice * dev = m_arduinoMgr.FindDevice(socket, true);

	string sensorPath = "arduino/";
	sensorPath += HexStringFromBytes(dev->GetUID(), ArduinoDevice::UID_SIZE);
	sensorPath += "/oneWire/";
	sensorPath += tempReader->GetOneWireAddr().ToString();

	
	m_cmdManager.SendCommand(
		&m_serverSocket,
		new SendSensorData(sensorPath.c_str(), tempReader->GetProbeValue()));
}


void CClient::OnArduinoUID(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg)
{
	ArduinoDevice * dev = m_arduinoMgr.FindDevice(socket, true);
	if (NULL != dev)
	{
		dev->SetUID(data, size);

		m_cmdManager.SendCommand(
			&m_serverSocket,
			new SendClientInfo);
	}
}