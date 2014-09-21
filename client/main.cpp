#include "pch.h"
#include "CommandManager.h"


bool done = false;
uint deadline = GetTickCount() + 1000;
uint curSensor = 0;


void OnEnumerationDone()
{
	for (uint i = 0; i < g_commMgr->GetOneWireDeviceCount(); ++i)
	{
		OneWireAddr addr = g_commMgr->GetOneWireDeviceID(i);

		g_commMgr->PushCommand(
			CMD_OW_READ_TEMP_SENSOR_DATA,
			addr.Address(),
			OneWireAddr::ADDR_LEN);
	}
	
}


int main()
{
	CSerialPort comPort;
	if (false == comPort.Open(ARDUINO_PORT))
	{
		printf("Failed to open %s", ARDUINO_PORT);
		return -1;
	}

	printf("Opened %s\n", ARDUINO_PORT);
	sleep(2000); // time to init microcontroller
	CommandManager commMgr(&comPort);

	// -------------------------------------------------------------------------

	bool readFlag = false;
	g_commMgr->PushCommand(CMD_REQUEST_ONE_WIRE_ENUM);
	
	while (true)
	{
		commMgr.Update();
		sleep(100);

		if (false == readFlag)
		{
			if (true == g_commMgr->IsOneWireEnumerated())
			{
				OnEnumerationDone();
				readFlag = true;
			}
		}
	}

	/*
	while (counter < exitBytes)
	{
		const uint r = comPort.Recv(buffer, 256);

		if (r < 256)
		{
			buffer[r] = 0;
			counter += r;

			//if (r > 0)
			{
				printf("%d: %s\n", r, buffer);
			}
		}

		sleep(1);
	}
	*/

	comPort.Close();
	return 0;
}
