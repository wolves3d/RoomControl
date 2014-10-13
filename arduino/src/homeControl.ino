// Includes
#include <Arduino.h>
#include "netCommand.h"
#include <OneWire.h>
#include "OneWireWrapper.h"
#include "pinWatchDog.h"

// Singletons
PinWatchDog pinWatchDog;
OneWireWrapper oneWire(ONE_WIRE_SIGNAL_PIN);

unsigned long loopCounter = 0;

Alarm relayAlarm(100);
int relayPins[4];
int curRelay = 0;
int mode = LOW;

// =============================================================================

class CommandManager
{
	#define MAX_PACK_SIZE 16
	byte m_buffer[MAX_PACK_SIZE];
	byte m_writeOffset;
	byte m_readOffset;
	byte m_size;
	byte m_targetSize;

	byte m_cmdID;
	bool m_waitingForData;


public:
	CommandManager()
		: m_writeOffset(0)
		, m_readOffset(0)
		, m_size(0)
		, m_targetSize(2)
		, m_waitingForData(true)
	{
	}

	bool PinWrite()
	{
		byte pinID = PopByte();
		byte pinValue = PopByte();

		pinMode(pinID, OUTPUT);
		digitalWrite(pinID, pinValue);

		return true;
	}

	bool OneWireReadTempData()
	{
		const byte buffSize = OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN;
		byte resBuffer[buffSize];
		memset(resBuffer, 0, buffSize);


// 		if (false == WaitForData(resBuffer, OneWireAddr::ADDR_LEN))
// 			return false;

		// hack objects
		//OneWireAddr * blob = (OneWireAddr *)resBuffer;
		byte * sensorData = resBuffer + OneWireAddr::ADDR_LEN;

		for (byte i = 0; i < OneWireAddr::ADDR_LEN; ++i)
		{
			resBuffer[i] = PopByte();
		}

		if (false == oneWire.ReadTemperatureData(sensorData, resBuffer, OneWireAddr::DATA_LEN))
			return false;

		SerialCommand::Send(RSP_OW_TEMP_SENSOR_DATA, resBuffer, (OneWireAddr::ADDR_LEN + OneWireAddr::DATA_LEN));
		return true;
	}


	void OnCommand(byte cmdID)
	{
		bool succeedFlag = false;
		
		switch (cmdID)
		{
			case CMD_PIN_WRITE: succeedFlag = PinWrite(); break;
			case CMD_REQUEST_ONE_WIRE_ENUM: succeedFlag = oneWire.Enumerate(); break;
			case CMD_OW_READ_TEMP_SENSOR_DATA: succeedFlag = OneWireReadTempData(); break;

			default:
				SerialCommand::Send(RSP_INVALID_CMD, (byte *)&cmdID, 2);
				return;
		}

		if (false == succeedFlag)
		{
			SerialCommand::Send(RSP_INVALID_REQUEST, (byte *)&cmdID, 2);
		}
	}

	byte ByteCount()
	{
		return m_size;
	}

	void PushByte(byte value)
	{
		m_buffer[m_size] = value;
		++m_size;
	}

	byte PopByte()
	{
		byte result = 0xFF;

		if (m_size > 0)
		{	
			result = m_buffer[0];
			--m_size;

			// move rest
			for (byte i = 0; i < m_size; ++i)
			{
				m_buffer[i] = m_buffer[i+1];
			}
		}


		return result;
	}



	void OnLoop()
	{
		if (true == Serial.available())
		{
			PushByte( Serial.read() );
		}
		
		if (ByteCount() >= m_targetSize)
		{
			if (true == m_waitingForData)
			{
				// command recieved
				m_cmdID = PopByte();

				// command argument size
				m_targetSize = PopByte();
				m_waitingForData = false;
			}
			else
			{
				// ready to call command
				OnCommand(m_cmdID);

				// prepare for next
				m_waitingForData = true;
				m_targetSize = 2;
			}
		}

		// ------
		
		if (true == oneWire.IsEnumerating())
		{
			OneWireAddr ow_blob;
			oneWire.GetNextAddr(&ow_blob);
		}
	}
};


CommandManager commandManager;

// =============================================================================


class StateBlink
{
	Alarm m_secondAlarm;
	Alarm m_ledOffAlarm;
	int	m_blinkDelay;
	int m_workTime;

public:

	enum EState
	{
		ES_FAIL = 0,
		ES_OK,
	};

	StateBlink()
		: m_blinkDelay(100)
		, m_workTime(0)
	{
		m_secondAlarm.Schedule(1000);
	}

	void SetState(EState state)
	{
		switch (state)
		{
			case StateBlink::ES_FAIL:
				m_blinkDelay = 900;
			break;

			case StateBlink::ES_OK:
				m_blinkDelay = 100;
			break;

			default:
				m_blinkDelay = 100;
		}
	}

	void OnUpdate()
	{
		// Pin 13 has an LED connected on most Arduino boards.
		#define STATE_LED 13

		if (true == m_ledOffAlarm.CheckAlarm())
		{
			pinMode(STATE_LED, OUTPUT);
			digitalWrite(STATE_LED, LOW);
			m_ledOffAlarm.Reset();
		}

		if (true == m_secondAlarm.CheckAlarm())
		{
			m_workTime++;
			SerialCommand::Send(RSP_PING, (byte *)&m_workTime, 2);


			pinMode(STATE_LED, OUTPUT);
			digitalWrite(STATE_LED, HIGH);

			m_ledOffAlarm.Schedule(m_blinkDelay);
		}
	}
};


StateBlink stateBlink;

// =============================================================================

void loop()
{
	stateBlink.OnUpdate();
	commandManager.OnLoop();



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



void setup(void)
{
	stateBlink.SetState( StateBlink::ES_OK );

	Serial.begin(9600);
	randomSeed(0);

	pinWatchDog.SleepTime(20); // 1000/20 = 50 analog-reads per second
	/*
	pinWatchDog.WatchFor(0);
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
}`