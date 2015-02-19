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