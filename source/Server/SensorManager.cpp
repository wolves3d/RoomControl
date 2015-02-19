#include "pch.h"
#include "MySqlClient.h"
#include "SensorManager.h"
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

	const char minutePath[] = "minute";

	CMySqlResult result;
	if (true == m_dataBase->Query(&result, query))
	{
		// output table name
		printf("Sensors:\n");

		CMySqlRow row;
		while (true == result.GetNextRow(&row))
		{
			uint sensorID = row.GetInt(0);
			string sensorPath = row.GetString(1);
			float value = row.GetFloat(4);

			printf("id %d path %s value %f\n", sensorID, sensorPath.c_str(), value);

			CSensor * sensor = new CSensor(sensorID, sensorPath, &value);
			m_sensorMap[sensorID] = sensor;

			if (sensorPath == minutePath)
			{
				m_minuteSensor = sensor;
			};
		}
	}

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


bool CSensorManager::UpdateSensor(uint id, float newValue)
{
	CSensor * sensor = GetSensor(id);

	if (NULL == sensor)
		return false;

	if (sensor->GetValue() == newValue)
		return false;
	
	sensor->SetValue(newValue);
	
	string query = u_string_format("UPDATE `sensors` SET `numeric_val` = %f WHERE `id` = %d", newValue, sensor->GetID());
	m_dataBase->Query(NULL, query.c_str());

	query = u_string_format("INSERT INTO `sensor_history` (`sensor_id`, `value`) VALUES (%d, %f)", sensor->GetID(), newValue);
	m_dataBase->Query(NULL, query.c_str());

	printf("Sensor (id:%d) value updated to %f\n", sensor->GetID(), newValue);

	return true;
}


void CSensorManager::UpdateSystemSensors()
{
	time_t now = time(NULL);
	tm * tmLocal = localtime(&now);

	/*
	tmLocal->tm_year + 1900,
	tmLocal->tm_mon + 1,
	tmLocal->tm_mday,
	tmLocal->tm_hour,
	tmLocal->tm_min,
	*/

	UpdateSensor(m_minuteSensor->GetID(), (float)tmLocal->tm_min);
}