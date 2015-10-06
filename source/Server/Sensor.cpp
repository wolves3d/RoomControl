#include "pch.h"
//#include "MySqlClient.h"
#include "SensorManager.h"


CSensor::CSensor(uint id, const string & path, const float * optValue)
	: m_id(id)
	, m_path(path)
{
	if (NULL != optValue)
	{
		m_value = (*optValue);
	}
}


#define ARDUINO_PATH_PREFIX "arduino/"
#define ARDUINO_PREFIX_LEN sizeof(ARDUINO_PATH_PREFIX)


bool CArduinoSensor::IsArduinoSensor(const char *sensorPath)
{
	return (sensorPath == strstr(sensorPath, ARDUINO_PATH_PREFIX));
}


CArduinoSensor::CArduinoSensor(uint id, const string & path, const float * optValue)
	: CSensor(id, path, optValue)
{
	if (path.size() > (ARDUINO_PREFIX_LEN + 16))
	{
		m_arduinoUID.append(path.c_str() + ARDUINO_PREFIX_LEN - 1, 16);
	}

	if (path.size() > (ARDUINO_PREFIX_LEN + 16 + 1 + 4))
	{
		m_pinName.append(path.c_str() + ARDUINO_PREFIX_LEN - 1 + 16 + 1 + 4, 2);
	}
}



/*
	static string GetArduinoUID(const char *sensorPath)
	{
		string result;

		if (true == IsArduinoSensor(sensorPath))
		{
			uint pathLen = strlen(sensorPath);
			uint offset = strlen("arduino/");

			if ((pathLen - offset) > 16)
			{
				result.append(sensorPath + offset, 16);
			}
		}

		if (true == result.empty())
		{
			DEBUG_ASSERT(false);
		}

		return result;
	}


}



	static bool IsArduinoPin(const char *sensorPath)
	{
		uint offset = strlen("arduino/") + 17;
		uint pathLen = strlen(sensorPath);

		if (pathLen > offset)
		{
			const char * pinWord = (sensorPath + offset);
			return (pinWord == strstr(sensorPath, "pin/"));
		}

		return false;
	}

	static string GetArduinoPinName(const char *sensorPath)
	{
		string result;

		if (true == IsArduinoPin(sensorPath))
		{
			uint offset = strlen("arduino/pin/");
			result.append(sensorPath + offset, 2);
		}
		else
		{
			DEBUG_ASSERT(false);
		}

		return result;
	}

	static string GetArduinoUID(const char *sensorPath)
	{
		string result;

		if (true == IsArduinoSensor(sensorPath))
		{
			uint pathLen = strlen(sensorPath);
			uint offset = strlen("arduino/");

			if ((pathLen - offset) > 16)
			{
				result.append(sensorPath + offset, 16);
			}
		}

		if (true == result.empty())
		{
			DEBUG_ASSERT(false);
		}

		return result;
	}
};*/