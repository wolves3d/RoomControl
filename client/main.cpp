#include "pch.h"
#include "serialport.h"


int main()
{
	const int exitBytes = 4096;
	int counter = 0;
	char buffer[256];

	CSerialPort comPort;
	if (true == comPort.Open(ARDUINO_PORT))
	{
		printf("Opened %s\n", ARDUINO_PORT);

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

			sleep(1000);
		}

		comPort.Close();
	}
	else
	{
		printf("Failed to open %s", ARDUINO_PORT);
	}
	
	return 0;
}
