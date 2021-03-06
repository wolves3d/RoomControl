#include "pch.h"
#include "Server.h"


CServer * g_server = NULL;


CServer::CServer(int port)
	: m_netListener(this)
	, m_cmdMgr(new CNetPacket, false)
{
	g_server = this;
	m_netListener.Init(port);
	//m_cmdMgr.RegisterHandler(new GetClientGUID());
	m_cmdMgr.RegisterHandler(new OnClientInfo());
	m_cmdMgr.RegisterHandler(new OnClientSensorUpdate());

	printf("Connecting to DB...");
	m_dataBase.Connect("192.168.0.1", "root", "trust#1sql", "smart_home");
	m_sensorManager.Init(&m_dataBase);
	m_ruleManager.Init();
}


void CServer::AddArduinoRecord(const byte * UID, size_t uidSize, IAbstractSocket * socket)
{
	string uidString = HexStringFromBytes(UID, uidSize);

	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(uidString);
	if (m_arduinoSockets.end() != it)
	{
		DEBUG_MSG("arduino UID duplicate");
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


IAbstractSocket * CServer::GetArduinoSocket(const byte * UID, size_t uidSize)
{
	string uidString = HexStringFromBytes(UID, uidSize);
	
	ArduinoSocketMap::const_iterator it = m_arduinoSockets.find(uidString);
	if (m_arduinoSockets.end() != it)
	{
		return (it->second);
	}

	return NULL;
}