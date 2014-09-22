#include "pch.h"
#include "CommandManager.h"


void LogDB(const char * sensorID, float sensorT);


CommandManager * g_commMgr = NULL;


CommandManager::CommandManager(CSerialPort * port)
	: m_port(port)
	, m_waitForAnswerMode(false)
	, m_isOneWireEnumerated(false)
{
	g_commMgr = this;
}


void CommandManager::PushCommand(ECommandID cmd, void * data, uint byteCount)
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


bool CommandManager::TrySendCommand()
{
	if (0 == m_commandList.size())
		return false;

	const Command & command = m_commandList[0];
	const uint packetSize = command.GetSize();
	bool result = false;

	if (packetSize == m_port->Send(&command, packetSize))
	{
		// 			// write args (if any)
		// 			byte * srcBuffer = (byte *)data;
		// 			while (byteCount--)
		// 			{
		// 				m_port->Send(srcBuffer, 1);
		// 				++srcBuffer;
		// 			}

		result = true;
	}

	if (true == result)
	{
		printf("SendCommand (id: %d) succeed\n", command.cmd);
	}
	else
	{
		printf("SendCommand (id: %d) failed\n", command.cmd);
	}

	m_commandList.erase( m_commandList.begin() );
	return result;
}


bool CommandManager::GetMoreData(void * buffer, uint byteCount)
{
	const uint readBytes = m_port->Recv(buffer, byteCount);
	return true;
}


uint CommandManager::GetOneWireDeviceCount()
{
	if (false == IsOneWireEnumerated())
		return 0;

	return m_owDeviceList.size();
}


const OneWireAddr & CommandManager::GetOneWireDeviceID(uint device)
{
	return m_owDeviceList[device];
}


void CommandManager::Update()
{
	if (false == m_waitForAnswerMode)
	{
		m_waitForAnswerMode = TrySendCommand();
	}

	ECommandID cmdID = CMD_NOP;
	const uint readBytes = m_port->Recv(&cmdID, 2);
	if (0 == readBytes)
		return;

	switch (cmdID)
	{
	case RSP_ONE_WIRE_ENUM_BEGIN:
		m_owDeviceList.clear();
		printf("RSP: 1wire enum begin\n");
		break;

	case RSP_ONE_WIRE_ROM_FOUND:
		{

			byte addr[8];
			uint r = m_port->Recv(addr, 8);

			printf("RSP: 1wire rom found addr ");
			for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
				printf("%02X ", addr[i]);
			printf("\n");

			m_owDeviceList.push_back( OneWireAddr(addr) );

			//g_commMgr->SendCommand(CMD_OW_READ_TEMP_SENSOR_DATA, addr, 8);
		}
		break;

	case RSP_ONE_WIRE_ENUM_END:
	{
		printf("RSP: 1wire enumerated!\n");
		m_isOneWireEnumerated = true;
	}
	break;

	case RSP_OW_TEMP_SENSOR_DATA:
		{
			const uint buffSize = OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN;
			byte dstBuffer[buffSize];
			memset(dstBuffer, 0, buffSize);
			g_commMgr->GetMoreData(dstBuffer, buffSize);

			// hack objects
			OneWireAddr * addr = (OneWireAddr *)dstBuffer;
			byte * data = dstBuffer + OneWireAddr::ADDR_LEN;

			printf("RSP: sensor data id:");
			for (uint i = 0; i < OneWireAddr::ADDR_LEN; ++i)
				printf("%02X ", dstBuffer[i]);

			printf("data: ");
			for (uint i = 0; i < OneWireAddr::DATA_LEN; ++i)
				printf("%02X ", dstBuffer[OneWireAddr::ADDR_LEN + i]);

			int raw = (data[1] << 8) | data[0];

			switch (addr->GetChipID())
			{
				case ECID_DS1822:
				case ECID_DS18B20:
				{
					byte cfg = (data[4] & 0x60);
					// at lower res, the low bits are undefined, so let's zero them
					if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
					else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
					else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
					//// default is 12 bit resolution, 750 ms conversion time
				}
				break;

				case ECID_DS18S20:
				{
					raw = raw << 3; // 9 bit resolution default
					if (data[7] == 0x10)
					{
						// "count remain" gives full 12 bit resolution
						raw = (raw & 0xFFF0) + 12 - data[6];
					}
				}
				break;

				default:
					printf("UNKNOWN CHIP ID!");
			}

			float celsius = (float)raw / 16.f;
			printf("Temperature = %02.02fC / %02.02fF\n", celsius, (32 + (1.8f * celsius)));

			const string sensorID( addr->ToString() );
			LogDB(sensorID.c_str(), celsius);
		}
		break;

	case RSP_INVALID_REQUEST:
		printf("RSP: invalid request\n");
		break;

	case RSP_PING:
		{
			uint workTime = 0;
			g_commMgr->GetMoreData(&workTime, 2);
			//printf("ping: work time %d sec\n", workTime);
		}
		break;

	case RSP_INVALID_CMD:
		printf("RSP_INVALID_CMD: %02x %02x\n", *(byte *)(&cmdID), *(byte *)((&cmdID) + 1));
		break;

	default:
		printf("DEFAULT: %s (%d bytes)\n", (char *)&cmdID, readBytes);
		break;
	}

	m_waitForAnswerMode = false;
}