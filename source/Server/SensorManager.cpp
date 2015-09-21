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


bool CSensorManager::UpdateSensor(uint id, float newValue)
{
	CSensor * sensor = GetSensor(id);

	if (NULL == sensor)
		return false;

	if (sensor->GetValue() == newValue)
	{
		// Value is actual - early quit
		return false;
	}
	
	// update memory record
	sensor->SetValue(newValue);
	
	// update data base record -------------------------------------------------
	
	string query = u_string_format("UPDATE `sensors` SET `numeric_val` = %f WHERE `id` = %d", newValue, sensor->GetID());
	m_dataBase->Query(NULL, query.c_str());

	query = u_string_format("INSERT INTO `sensor_history` (`sensor_id`, `value`) VALUES (%d, %f)", sensor->GetID(), newValue);
	m_dataBase->Query(NULL, query.c_str());

	printf("Sensor (id:%d, path:%s) value updated to %f\n", sensor->GetID(), sensor->GetPath(), newValue);
	
	// send client command -----------------------------------------------------

	const char * sensorPath = sensor->GetPath();

	// FIXME: hack
	if (sensorPath == strstr(sensorPath, "arduino"))
	{
		// FIXME: hack
		const char *pinName = strstr(sensorPath, "/pin/");
		if (NULL != pinName)
		{
			pinName += 5;

			// FIXME: hack
			IAbstractSocket * socket = g_server->GetArduinoSocket((const byte *)"\x0\x1\x2\x3\x4\x5\x6\x7", 8);
			if (NULL != socket)
			{
				SetPinValue pinSetter(pinName, (byte)sensor->GetValue());

				CCommandManager * cmdMgr = g_server->GetCommandManager();
				cmdMgr->SendCommand(socket, &pinSetter);
			}
		}
	}

	return true;
}


void CSensorManager::UpdateSystemSensors()
{
	time_t now = time(NULL);
	tm * tmLocal = localtime(&now);

	UpdateSensor(m_secSensorID, (float)tmLocal->tm_sec);
	UpdateSensor(m_minuteSensorID, (float)tmLocal->tm_min);
	UpdateSensor(m_hourSensorID, (float)tmLocal->tm_hour);
	UpdateSensor(m_daySensorID, (float)tmLocal->tm_mday);
	UpdateSensor(m_weekDaySensorID, (float)tmLocal->tm_wday);
	UpdateSensor(m_monthSensorID, (float)tmLocal->tm_mon + 1);
	UpdateSensor(m_yearSensorID, (float)tmLocal->tm_year + 1900);
}