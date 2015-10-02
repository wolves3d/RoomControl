class StateBlink
{
	Alarm m_secondAlarm;
	Alarm m_toggleLedAlarm;
	int	m_blinkDelay;
	int m_workTime;

public:

	enum EState
	{
		ES_FAIL = 0,
		ES_OK,
	};

	StateBlink()
		: m_workTime(0)
	{
		SetState(ES_OK);
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
			m_blinkDelay = 20;
			break;

		default:
			m_blinkDelay = 100;
		}
	}

	void OnUpdate()
	{
		// Pin 13 has an LED connected on most Arduino boards.
		#define STATE_LED 13

		if (true == m_toggleLedAlarm.CheckAlarm())
		{
			pinMode(STATE_LED, OUTPUT);
			digitalWrite(STATE_LED, LOW);
			m_toggleLedAlarm.Reset();
		}

		if (true == m_secondAlarm.CheckAlarm())
		{
			m_workTime++;

			//SerialCommand::Send(RSP_PING, 0, (byte *)&g_queueSize, 1);
			SerialCommand::Send(RSP_PING, 0, (byte *)&m_workTime, 2);

			pinMode(STATE_LED, OUTPUT);
			digitalWrite(STATE_LED, HIGH);

			m_toggleLedAlarm.Schedule(m_blinkDelay);
		}
	}
};
