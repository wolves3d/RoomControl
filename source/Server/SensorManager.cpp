#include "pch.h"
#include "Server.h"
#include <time.h>


CSensorManager * g_sensorMgr = NULL;


CSensorManager::CSensorManager()
{
	g_sensorMgr = this;
}


bool CSensorManager::Init(CMySqlClient * dbConn)
{
	m_dataBase = dbConn;
	const char query[] = "SELECT * FROM `sensors` WHERE 1";

	CMySqlResult result;
	if (true == m_dataBase->Query(&result, query))
	{
		// output table name
		printf("Sensors list (DB)\n");

		CMySqlRow row;
		while (true == result.GetNextRow(&row))
		{
			uint sensorID = row.GetInt(0);
			string sensorPath = row.GetString(1);
			float value = row.GetFloat(4);

			printf("id %d path %s value %f\n", sensorID, sensorPath.c_str(), value);

			CSensor * sensor = new CSensor(sensorID, sensorPath, &value);
			m_sensorMap[sensorID] = sensor;
		}
	}

	// -------------------------------------------------------------------------

	m_secSensorID = GetSensorIdByPath("second");
	m_minuteSensorID = GetSensorIdByPath("minute");
	m_hourSensorID = GetSensorIdByPath("hour");
	m_daySensorID = GetSensorIdByPath("day");
	m_weekDaySensorID = GetSensorIdByPath("weekday");
	m_monthSensorID = GetSensorIdByPath("month");
	m_yearSensorID = GetSensorIdByPath("year");

	return true;
}


CSensor * CSensorManager::GetSensor(uint id)
{
	SensorMap::iterator it = m_sensorMap.find(id);

	if (m_sensorMap.end() != it)
	{
		return (it->second);
	}

	return NULL;
}


uint CSensorManager::GetSensorIdByPath(const char * szSensorPath)
{
	string query = u_string_format("SELECT `id` FROM `sensors` WHERE `path` = '%s'", szSensorPath);

	CMySqlResult result;
	if (true == m_dataBase->Query(&result, query.c_str()))
	{
		CMySqlRow row;
		if (true == result.GetNextRow(&row))
		{
			return row.GetInt(0);
		}
	}

	return -1;
}



class CSensorPath // FIXME: create CArduinoSensor
{
public:
	static bool IsArduinoSensor(const char *sensorPath)
	{
		return (sensorPath == strstr(sensorPath, "arduino/"));
	}

	static bool IsArduinoPin(const char *sensorPath)
	{
		return (sensorPath == strstr(sensorPath, "arduino/pin/"));
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
};




bool CSensorManager::UpdateSensor(uint id, float newValue, bool isSetter, bool forceUpdateClient)
{
	CSensor * sensor = GetSensor(id);

	if (NULL == sensor)
		return false;

	const bool isValueChanged = (sensor->GetValue() != newValue);
	if (true == isValueChanged)
	{
		// update memory record
		sensor->SetValue(newValue);

		// update data base record -------------------------------------------------

		if (0 != strcmp("second", sensor->GetPath())) // Prevent every second DB flood
		{
			const size_t sensorID = sensor->GetID();
			/*
			string query = u_string_format(
			"UPDATE `sensors` SET `numeric_val`=%f WHERE `id`=%d; INSERT INTO `sensor_history` (sensor_id, value) VALUES('%d', '%f')",
			newValue, sensor->GetID(), sensor->GetID(), newValue);

			m_dataBase->Query(NULL, query.c_str());
			*/
			string query = u_string_format("UPDATE `sensors` SET `numeric_val` = %f WHERE `id` = %d;",
				newValue, sensorID);

			m_dataBase->Query(NULL, query.c_str());

			query = u_string_format("INSERT INTO `sensor_history` (`sensor_id`, `value`) VALUES(%d, %f)",
				sensorID, newValue);

			m_dataBase->Query(NULL, query.c_str());
		
			printf("Sensor (id:%llu, path:%s) value updated to %f\n", (unsigned long long)sensor->GetID(), sensor->GetPath(), newValue);
		}
	}

	// send client command -----------------------------------------------------

	if (forceUpdateClient || (isValueChanged && isSetter))
	{
		const char * sensorPath = sensor->GetPath();
		if (true == CSensorPath::IsArduinoSensor(sensorPath))
		{
			if (true == CSensorPath::IsArduinoPin(sensorPath))
			{
				string pinName = CSensorPath::GetArduinoPinName(sensorPath);
				if (false == pinName.empty())
				{
					const string uidString = CSensorPath::GetArduinoUID(sensorPath);
					IAbstractSocket * socket = g_server->GetArduinoClient(uidString);
					if (NULL != socket)
					{
						SetPinValue *pinSetter = NEW SetPinValue(pinName, (byte)sensor->GetValue());

						CCommandManager * cmdMgr = g_server->GetCommandManager();
						cmdMgr->SendCommand(socket, pinSetter);
					}
				}
			}
		}
	}
	
	return true;
}


void CSensorManager::UpdateSystemSensors()
{
	time_t now = time(NULL);
	tm * tmLocal = localtime(&now);

	UpdateSensor(m_secSensorID, (float)tmLocal->tm_sec, false);
	UpdateSensor(m_minuteSensorID, (float)tmLocal->tm_min, false);
	UpdateSensor(m_hourSensorID, (float)tmLocal->tm_hour, false);
	UpdateSensor(m_daySensorID, (float)tmLocal->tm_mday, false);
	UpdateSensor(m_weekDaySensorID, (float)tmLocal->tm_wday, false);
	UpdateSensor(m_monthSensorID, (float)tmLocal->tm_mon + 1, false);
	UpdateSensor(m_yearSensorID, (float)tmLocal->tm_year + 1900, false);
}