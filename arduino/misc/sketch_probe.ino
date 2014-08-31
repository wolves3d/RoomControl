#include <OneWire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

enum
{
  CMD_PIN_WRITE,
  CMD_PIN_READ,
  
  CMD_PIN_WATCH,	// Отслеживать изменение значения pin
  CMD_PIN_STOP_WATCH,	// Прекратить отслеживать значения pin
  RSP_ON_PIN_CHANGED,	// Ответ при определении изменения
  
  CMD_REQUEST_ONE_WIRE_ENUM,
  CMD_ONE_WIRE_ENUM_BEGIN,
  CMD_ONE_WIRE_ROM_FOUND,
  CMD_ONE_WIRE_ENUM_END,
  
  CMD_INVALID_REQUEST,
};

#define ANALOG_PIN_COUNT 15

class PinWatchDog
{
	byte pinFlags[ANALOG_PIN_COUNT];
	uint16_t pinValues[ANALOG_PIN_COUNT];
	
public:
	PinWatchDog()
	{
		for (byte i = 0; i < ANALOG_PIN_COUNT; ++i)
		{
			pinFlags[i] = 0;
			pinValues[i] = 0;
		}
	}
	
	bool WatchFor(byte pin)
	{
		if (pin < ANALOG_PIN_COUNT)
		{
			if (0 == pinFlags[pin])
			{
				pinFlags[pin] = 1;
				pinValues[pin] = analogRead(pin);
				return true;
			}
		}
		
		return false;
	}
	
	bool StopWatchFor(byte pin)
	{
		if (pin < ANALOG_PIN_COUNT)
		{
			if (0 != pinFlags[pin])
			{
				pinFlags[pin] = 0;
				return true;
			}
		}
		
		return false;
	}
	
	void OnUpdate()
	{
		for (byte i = 0; i < ANALOG_PIN_COUNT; ++i)
		{
			if (0 != pinFlags[i])
			{
				const uint16_t currentPinValue = analogRead(i);
				if (currentPinValue != pinValues[i])
				{
					pinValues[i] = currentPinValue;
					
					int arguments[2] = { i, currentPinValue };
					SendCommand(RSP_ON_PIN_CHANGED, (byte *)arguments, 2 * sizeof(int));
				}
			}
		}
	}
};


#define PIN_ONE_WIRE 10 // (a 4.7K resistor is necessary)
OneWire  ds(PIN_ONE_WIRE);

//PinWatchDog  pinWatchDog;
unsigned long alarmTime;

int relayPins[4];

void setup(void)
{
  relayPins[0] = 2;
    relayPins[1] = 4;
      relayPins[2] = 7;
        relayPins[3] = 8;
        
  	Serial.begin(9600);
        randomSeed(0);
        alarmTime = millis() + 500;
//        Serial.print("Alarm = ");
//        Serial.println(time);	

//        OneWireEnum();
}
/*
int main()
{
	setup();
	Serial.println("Hello world!");

	while (true)
	{
//                delay(15);
Serial.println(millis(), HEX);
//		loop();
	}

	
	return 0;
}
*/

void OnFrame()
{
	// ping controller
}


//void OnCommand(byt
/*
void OneWireWrite(byte * addr, byte value, byte power)
{
}
*/
void OneWireRead(byte * buffer, const byte * addr, byte count)
{
//	byte present = ds.reset();
//	ds.select(addr);

	for (byte i = 0; i < count; ++i)
	{
		buffer[i] = ds.read();
	}
}


void OnCommand(uint16_t cmdID)
{
	bool succeedFlag = false;
	
	switch (cmdID)
	{
		case CMD_REQUEST_ONE_WIRE_ENUM:	succeedFlag = OneWireEnum();	break;
	}

	if (false == succeedFlag)
	{
		SendCommand(CMD_INVALID_REQUEST, (byte *)&cmdID, 2);
	}
}


void SendCommand(uint16_t cmdID, byte * data, byte count)
{
	Serial.write((byte *)&cmdID, 2);

        if (count > 0)
        {
                Serial.write(data, count);
        }
}


bool OneWireEnum()
{
	byte addr[8];
	ds.reset_search();
	//delay(250);

	//Serial.print("1-Wire enum begin");
        SendCommand(CMD_ONE_WIRE_ENUM_BEGIN, 0, 0);

	while (ds.search(addr))
	{
		SendCommand(CMD_ONE_WIRE_ROM_FOUND, addr, 8);
/*
		Serial.print("found: ");

		for (byte i = 0; i < 8; i++)
		{
			Serial.write(' ');
			Serial.print(addr[i], HEX);
		}
*/
	}

//	Serial.print("1-Wire enum end");
        SendCommand(CMD_ONE_WIRE_ENUM_END, 0, 0);
	return true;
}


int curRelay = 0;
int mode = LOW;

void loop()
{
//	pinWatchDog.OnUpdate();

  unsigned long currentTime = millis();
  
  if (currentTime > alarmTime)
  {
    Serial.write("bump ");
    alarmTime = (currentTime + 500);
    
   int relayPin = relayPins[ curRelay ];
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, mode);    
    /*
  if (LOW == mode)
  {
    mode = HIGH;
  }
  else
  {*/

        
       ++curRelay;
      if (curRelay > 3)
      {
        curRelay = 0;
        
              mode = (LOW == mode)
        ? HIGH
        : LOW;
      }
     
//  }
 
   
    /*
   
    if (0 == random(2))
    {
      digitalWrite(relayPin, HIGH);
       Serial.println("hi");
    }
    else
    {
      digitalWrite(relayPin, LOW);
             Serial.println("low");
    }*/
  }
  return;


  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(750);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

      Serial.println("A");

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

      Serial.println("B");
  
//  delay(750);     // maybe 750ms is enough, maybe not
        Serial.println("C");
        
        
  // we might do a ds.depower() here, but the reset will take care of it.
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  
//  OneWireRead(data, addr, 9);
  
  
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
} 
