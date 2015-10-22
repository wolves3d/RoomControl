#include "pch.h"
#include "Server.h"


CServer * g_server = NULL;


CServer::CServer(int port)
	: m_netListener(this)
	, m_cmdMgr(new CNetPacket, false)
	, m_pipeDesc(0)
{
	g_server = this;
	m_netListener.Init(port);
	//m_cmdMgr.RegisterHandler(new GetClientGUID());
	m_cmdMgr.RegisterHandler(new OnClientInfo());
	m_cmdMgr.RegisterHandler(new OnClientSensorUpdate());

	LOG_INFO("Connecting to DB...");
	if (true == m_dataBase.Connect("192.168.0.1", "root", "trust#1sql", "smart_home"))
	{
		LOG_INFO("OK");
	}
	else
	{
		LOG_INFO("FAILED");
	}

	m_sensorManager.Init(&m_dataBase);
	m_ruleManager.Init();
}


void CServer::AddArduinoRecord(const byte * UID, size_t uidSize, IAbstractSocket * socket)
{
	string uidString = HexStringFromBytes(UID, uidSize);

	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(uidString);
	if (m_arduinoSockets.end() != it)
	{
		LOG_ERROR("arduino UID duplicate");
	}

	m_arduinoSockets[uidString] = socket;

	// -------------------------------------------------------------------------

	string arduinoPath = "arduino/" + uidString;
	string query = u_string_format("SELECT `id` FROM `sensors` WHERE `path` LIKE '%s/pin/%%'", arduinoPath.c_str());

	CMySqlResult result;
	if (true == GetDB()->Query(&result, query.c_str()))
	{
		CMySqlRow row;
		while (true == result.GetNextRow(&row))
		{
			const uint sensorID = row.GetInt(0);
			CSensor *sensor = g_sensorMgr->GetSensor(sensorID);

			g_sensorMgr->UpdateSensor(sensorID, sensor->GetValue(), true, true);

			/*
			uint sensorA_ID = row.GetInt(2);
			uint logicOpID = row.GetInt(3);
			uint sensorB_ID = row.GetInt(4);
			uint isInverted = row.GetInt(5);
			uint targetSensorID = row.GetInt(6);

			ILogicOp * logicOp = GetLogicOpWithID(logicOpID);
			ISensor * sensorA = g_sensorMgr->GetSensor(sensorA_ID);
			ISensor * sensorB = g_sensorMgr->GetSensor(sensorB_ID);

			printf("Rule id:%d (sensorA:%d) %s (sensorB:%d)\n",
				ruleID, sensorA_ID, logicOp->GetName(), sensorB_ID);

			CRule * rule = new CRule(ruleID, targetSensorID);
			rule->SetInverted(0 != isInverted);
			rule->SetOperands(sensorA, logicOp, sensorB);

			if (false == AddRule(rule))
			{
				DEL(rule);
				}
				*/
		}
	}
	/*
	// Set all pins here

	Select all sensors affiliated with arduino
	Set all pins
	*/
}


IAbstractSocket * CServer::GetArduinoClient(const string &arduinoUID)
{
	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(arduinoUID);
	if (m_arduinoSockets.end() != it)
	{
		return (it->second);
	}

	return NULL;
}


void CServer::OnUpdate()
{
	m_sensorManager.UpdateSystemSensors();
	m_ruleManager.OnUpdate();

	m_cmdMgr.OnUpdate();
	m_netListener.OnUpdate();

	UpdatePipe();
}


void CServer::OpenPipe()
{

#ifndef WIN32
	LOG_INFO("Creating pipe...");
	if (0 != mkfifo(PIPE_NAME, 0777))
	{
		LOG_INFO("FAILED");
	}
	else
	{
		LOG_INFO("Pipe %s created", PIPE_NAME);

		m_pipeDesc = open(PIPE_NAME, O_RDONLY | O_NONBLOCK);
//		if (m_pipeDesc <= 0)
	}
#endif // #ifndef WIN32

}


void CServer::UpdatePipe()
{
#ifndef WIN32

	const uint bufLen = 32;
	byte buffer[bufLen];

	if (m_pipeDesc > 0)
	{
		memset(buffer, 0, bufLen);
		if (read(m_pipeDesc, buffer, bufLen) > 0)
		{
			LOG_INFO("Got read from pipe! :)", PIPE_NAME);
		}
	}

#endif // #ifndef WIN32
}