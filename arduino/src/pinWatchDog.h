
class Alarm
{
	unsigned long m_deadLine;
	int m_delay;

public:

	Alarm()
	{
		Reset();
	}

	Alarm(int delay)
	{
		Reset();
		Schedule(delay);
	}

	void Reset()
	{
		m_deadLine = 0;
	}

	void Schedule(int delay, int timeCorrection = 0)
	{
		m_delay = delay;
		m_deadLine = millis() + m_delay + timeCorrection;
	}

	bool CheckAlarm(bool reschedule = true)
	{
		if (0 != m_deadLine)
		{
			unsigned long currentTime = millis();

			if (currentTime >= m_deadLine)
			{
				if (true == reschedule)
				{
					// Auto reschedule (with error compensation)
					Schedule(m_delay, -(int)(currentTime - m_deadLine));
				}
				else
				{
					// Stopped till call Alarm.Schedule
					m_deadLine = 0;
				}

				return true;
			}
		}

		return false;
	}
};


class PinWatchDog
{
	Alarm m_sleepTimer;
	byte pinFlags[ANALOG_PIN_COUNT];
	uint16_t pinValues[ANALOG_PIN_COUNT];
	
public:
	PinWatchDog()
		: m_sleepTimer(0)
	{
		for (byte i = 0; i < ANALOG_PIN_COUNT; ++i)
		{
			pinFlags[i] = 0;
			pinValues[i] = 0;
		}
	}

	void SleepTime(int sleepTime)
	{
		m_sleepTimer.Schedule(sleepTime);
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
		if (true == m_sleepTimer.CheckAlarm())
		{
			for (byte i = 0; i < ANALOG_PIN_COUNT; ++i)
			{
				if (0 != pinFlags[i])
				{
					const uint16_t currentPinValue = analogRead(i);
					if (currentPinValue != pinValues[i])
					{
						pinValues[i] = currentPinValue;
					
						//int arguments[2] = { i, currentPinValue };
						SerialCommand::Send(RSP_PING, 0, (byte *)&currentPinValue, 2);
						//SerialCommand::Send(RSP_ON_PIN_CHANGED, (byte *)arguments, 2 * sizeof(int));
					}
				}
			}
		}
	}
};