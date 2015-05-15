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


class ReadEEPROM : public INetCommand
{
	COMMAND_HEADER(ReadEEPROM, CMD_READ_EEPROM, RSP_READ_EEPROM);

	virtual uint OnFillData(void * buffer, uint maxByteCount)
	{
		byte * buf = (byte *)buffer;
		buf[0] = 0; // EEPROM offset
		buf[1] = 8; // byte count to read from EEPROM
		return 2;
	}

	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		printf("RSP: EEPROM");

		for (uint i = 0; i < size; ++i)
		{
			printf(" %02X", data[i]);
		}

		printf("\n");
	}
};


class ClientGetGUIDResponse : public IResponseHandler
{
	HANDLER_HEADER(ClientGetGUIDResponse, CMD_GET_CLIENT_GUID)
	{
		printf("client: response - get client GUID\n");

		vector <ArduinoDevice *> deviceList = g_arduinoManager->BuildDeviceList();
		uint count = deviceList.size();
		printf("\tarduino device count: %d\nenumeration engaged\n", count);

		// 0. Каждой команде нужен тег (уникальный!)
		// 0. К тегу привязывается колл-бэк (опционально)
		// 1. Создать специальную команду

		ArduinoDeviceEnumeration * enumCallback = new ArduinoDeviceEnumeration(count, mgr);

		for (uint i = 0; i < count; ++i)
		{
			ArduinoDevice * device = deviceList[i];

			ReadEEPROM * command = new ReadEEPROM;
			command->SetCallback(enumCallback);
			//command->SetTag(tag); // тег генерировать автоматически (инкремент при отправке команды)
//			g_arduinoCmdManager->AddCatcher(device->GetPort(), RSP_READ_EEPROM, tag, enumCallback);
//			g_arduinoCmdManager->AddCatcher(device->GetPort(), &command, enumCallback);

			// 	byte args[] = { 0, 8 };
			// 	arduinoCmdManager.SendCommand(&arduinoPort, CMD_READ_EEPROM, &args, sizeof(args));

			// 2. Запихнуть в неё колл-бэк на каждый вызов
			g_arduinoCmdManager->SendCommand(device->GetPort(), command);
		}
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


	CSerialPort * arduinoPort = new CSerialPort;
	if (true == arduinoPort->Open(ARDUINO_PORT))
	{
		System::SleepMS(2000); // time to init microcontroller
		m_arduinoCmdManager.GetPacketManager()->AddClent(arduinoPort);
	}

	ArduinoDevice * device = m_arduinoMgr.FindDevice(arduinoPort); // register device
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