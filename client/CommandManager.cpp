#include "pch.h"
#include "CommandManager.h"
#include "CommandHandlers.h"


void LogDB(const char * sensorID, float sensorT);


CommandManagerArduino * g_commMgr = NULL;


CommandManagerArduino::CommandManagerArduino(const char * portName) 
	: m_portName(portName)
	, m_waitForAnswerMode(false)
	, m_isOneWireEnumerated(false)
{
	memset(m_handlerTable, 0, sizeof(m_handlerTable));

	// FIXME: memory leak, no free on destruct
	RegisterHandler(new OneWireEnumBegin());
	RegisterHandler(new OneWireRomFound());
	RegisterHandler(new OneWireEnumEnd());
	RegisterHandler(new OneWireTemperature());
	RegisterHandler(new PingResponse());
	RegisterHandler(new ReadEEPROM());
	RegisterHandler(new WriteEEPROM());

	g_commMgr = this;

	Update();
}


bool CommandManagerArduino::CheckPort()
{
	if (false == m_port.IsValid())
	{
		if (false == m_port.Open(m_portName.c_str()))
		{
			printf("Failed to open %s\n", ARDUINO_PORT);
			return false;
		}
	}

	return true;
}


void CommandManagerArduino::PushCommand(ECommandID cmd, void * data, uint byteCount)
{
	Command command;
	command.cmd = cmd;
	command.byteCount = byteCount;

	if (byteCount > 0)
	{
		memcpy(command.args, data, byteCount);
	}

	m_commandList.push_back(command);
}


bool CommandManagerArduino::TrySendCommand()
{
	if (0 == m_commandList.size())
		return false;

	const Command & command = m_commandList[0];
	const uint packetSize = command.GetSize();
	bool result = false;

	int bytesWritten = m_port.Send(&command, packetSize);
	
	if (-1 == bytesWritten)
	{
		printf("Port write failed to write %d bytes, closing port\n", packetSize);
		m_port.Close();
	}
	else if (packetSize == bytesWritten)
	{
		result = true;
	}
	else
	{
		printf("Failed to write %d bytes, only %d written\n", packetSize, bytesWritten);
	}

	if (true == result)
	{
		m_commandList.erase( m_commandList.begin() );
	}
	
	return result;
}


bool CommandManagerArduino::GetMoreData(void * buffer, uint byteCount)
{
	const uint readBytes = m_port.Recv(buffer, byteCount);
	return true;
}


uint CommandManagerArduino::GetOneWireDeviceCount()
{
	if (false == IsOneWireEnumerated())
		return 0;

	return m_owDeviceList.size();
}


void CommandManagerArduino::AddOneWireDevice(OneWireAddr deviceAddr)
{
	m_owDeviceList.push_back(deviceAddr);
}

const OneWireAddr & CommandManagerArduino::GetOneWireDeviceID(uint device)
{
	return m_owDeviceList[device];
}


CommandHandler * CommandManagerArduino::GetHandlerWithID(ECommandID cmdID)
{
	return (true == CheckHandlerID(cmdID))
		? m_handlerTable[cmdID]
		: NULL;
}

void CommandManagerArduino::RegisterHandler(CommandHandler * handler)
{
	if (NULL == handler)
	{
		printf("RegisterHandler error: invalid handler");
		return;
	}

	ECommandID cmdID = handler->GetCommandID();

	if (false == CheckHandlerID(cmdID))
	{
		printf("RegisterHandler error: invalid handler ID");
		return;
	}
	
	if (NULL != GetHandlerWithID(cmdID))
	{
		printf("RegisterHandler error: handler %d already registred!", cmdID);
		return;
	}

	m_handlerTable[cmdID] = handler;
}


void CommandManagerArduino::ClearOneWireDeviceList()
{
	m_isOneWireEnumerated = false;
	m_owDeviceList.clear();

	printf("RSP: 1wire enum begin\n");
}


void CommandManagerArduino::OnOneWireEnumerated()
{
	m_isOneWireEnumerated = true;
	printf("RSP: 1wire enumerated!\n");
}


void CommandManagerArduino::Update()
{
	if (false == CheckPort())
	{
		// port problem
		printf("port problem\n");
		System::SleepMS(1000);
		return;
	}

	if (false == m_waitForAnswerMode)
	{
		m_waitForAnswerMode = TrySendCommand();
	}

	// -- reading response and args

	byte buf[256];
	memset(buf, 0, sizeof(buf));
	byte * argData = (buf + 2);
	ECommandID cmdID = CMD_NOP;
	uint readBytes = m_port.Recv(buf, 2);

	if (0 == readBytes)
	{
		// no input
		return;
	}

	if (2 != readBytes)
	{
		printf("protocol error!\n (%d bytes)", readBytes);
		return;
	}

	cmdID = (ECommandID)buf[0];
	uint size = buf[1];

	if (0 != size)
	{
		readBytes = m_port.Recv(argData, size);

		if (size != readBytes)
		{
			printf("data read error (%d bytes) expected %d bytes\n", readBytes, size);
			return;
		}
	}

	CommandHandler * handler = GetHandlerWithID(cmdID);
	if (NULL != handler)
	{
		handler->OnResponse(argData, size);
	}
	else
	{
		if (RSP_INVALID_REQUEST == cmdID)
		{
			printf("RSP: invalid request\n");
		}
		else
		{
			printf("NO HANDLER! %02x (%d bytes)\n", cmdID, readBytes);
		}
	}

	m_waitForAnswerMode = false;
}
