// Includes
#include <Arduino.h>
#include "netCommand.h"
#include <OneWire.h>
#include "OneWireWrapper.h"

#include "pinWatchDog.h"
#include "StateBlink.h"

#include <EEPROM.h>
#include "CommandManager.h"

// Singletons
PinWatchDog pinWatchDog;
CommandManager commandManager;
StateBlink stateBlink;

unsigned long loopCounter = 0;

Alarm relayAlarm(100);
int relayPins[4];
int curRelay = 0;
int mode = LOW;

// =============================================================================

void setup(void)
{
	stateBlink.SetState(StateBlink::ES_OK);

	Serial.begin(9600);
	randomSeed(0);

	pinWatchDog.SleepTime(500); // 1000/500 = 2 analog-reads per second
	pinWatchDog.WatchFor(0);

	/*
	pinWatchDog.WatchFor(1);
	pinWatchDog.WatchFor(2);
	pinWatchDog.WatchFor(3);
	pinWatchDog.WatchFor(4);
	pinWatchDog.WatchFor(5);
	pinWatchDog.WatchFor(6);
	*/

	relayPins[0] = 2;
	relayPins[1] = 4;
	relayPins[2] = 7;
	relayPins[3] = 8;

	//oneWire.Enumerate();
}

// =============================================================================

void loop()
{
	commandManager.OnLoop();
	stateBlink.OnUpdate();
	pinWatchDog.OnUpdate();
	



	/*
	++loopCounter;

	if (true == everySecondAlarm.CheckAlarm())
	{
		Serial.write("loops per/sec ");
		Serial.println(loopCounter);

		loopCounter = 0;
	}

	// --
	/*
	pinWatchDog.OnUpdate();

	if (true == relayAlarm.CheckAlarm())
	{  
		int relayPin = relayPins[ curRelay ];
		pinMode(relayPin, OUTPUT);
		digitalWrite(relayPin, mode);    
  
		++curRelay;
      if (curRelay > 3)
      {
        curRelay = 0;
        
              mode = (LOW == mode)
        ? HIGH
        : LOW;
      }
	}
     
	 */

  return;
 

	OneWireAddr ow_blob;
	if (false == oneWire.GetNextAddr(&ow_blob))
	{
		//Serial.println("searchDone!");
		oneWire.Enumerate();
	}
	else
	{			
		// Check ROM CRC
		if (false == ow_blob.CheckCRC())
		{
			Serial.println("CRC is not valid!");
		}
		else
		{
			byte data[12];
			oneWire.ReadTemperatureData(data, ow_blob.Address(), 12);
			
			// Convert the data to actual temperature
			// because the result is a 16 bit signed integer, it should
			// be stored to an "int16_t" type, which is always 16 bits
			// even when compiled on a 32 bit processor.
			int16_t raw = (data[1] << 8) | data[0];

			switch (ow_blob.GetChipID())
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
					Serial.print("UNKNOWN CHIP ID!");
				return;
			}

			float celsius = (float)raw / 16.f;
			Serial.print("Temperature = ");
			Serial.print(celsius);
			Serial.print(" Celsius, ");

// 				float fahrenheit = 32 + (1.8 * celsius);
// 				Serial.print(fahrenheit);
// 				Serial.println(" Fahrenheit");
			Serial.println();
		}
	}
}