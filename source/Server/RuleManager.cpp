#include "pch.h"
#include "Server.h"


class COddOperator : public ILogicOp
{
	virtual const char * GetName() const { return "odd"; }

	virtual bool Evaluate(ISensor *a, ISensor *b)
	{
		DEBUG_ASSERT(NULL != a);
		DEBUG_ASSERT(NULL == b);

		int test = ((int)a->GetValue() % 2);
		return (0 != test);
	}
};


void CRuleManager::Init()
{
	InitLogicOps();
	InitRules();
}


void CRuleManager::AddLogicOp(ILogicOp *logicOp)
{
	const string opName = logicOp->GetName();

	if (NULL == GetLogicOpWithName(opName.c_str()))
	{
		m_logicOpMapWithName[opName] = logicOp;
	}
	else
	{
		// error
		DEBUG_ASSERT(false);
	}
}


ILogicOp * CRuleManager::GetLogicOpWithName(const char * szName)
{
	map <string, ILogicOp *>::const_iterator it = m_logicOpMapWithName.find(szName);
	if (m_logicOpMapWithName.end() != it)
	{
		return (it->second);
	}

	return NULL;
}


ILogicOp * CRuleManager::GetLogicOpWithID(size_t opID)
{
	map <size_t, ILogicOp *>::const_iterator it = m_logicOpMapWithID.find(opID);
	if (m_logicOpMapWithID.end() != it)
	{
		return (it->second);
	}

	return NULL;
}


void CRuleManager::AddLogicOpWithID(size_t opID, ILogicOp *logicOp)
{
	if (NULL == GetLogicOpWithID(opID))
	{
		m_logicOpMapWithID[opID] = logicOp;
	}
	else
	{
		// error
		DEBUG_ASSERT(false);
	}
}


void CRuleManager::InitLogicOps()
{
	AddLogicOp(new COddOperator);

	CMySqlClient * db = g_server->GetDB();
	const char query[] = "SELECT * FROM `logic_ops` WHERE 1";

	CMySqlResult result;
	if (true == db->Query(&result, query))
	{
		// output table name
		printf("Logic operators list (DB)\n");

		CMySqlRow row;
		while (true == result.GetNextRow(&row))
		{
			uint opertaorID = row.GetInt(0);
			string opertaorName = row.GetString(1);

			printf("id %d name %s\n", opertaorID, opertaorName.c_str());

			ILogicOp * logicOp = GetLogicOpWithName(opertaorName.c_str());
			if (NULL != logicOp)
			{
				AddLogicOpWithID(opertaorID, logicOp);
			}
		}
	}
}


void CRuleManager::InitRules()
{
	AddLogicOp(new COddOperator);

	CMySqlClient * db = g_server->GetDB();
	const char query[] = "SELECT * FROM `rules` WHERE 1";

	CMySqlResult result;
	if (true == db->Query(&result, query))
	{
		// output table name
		printf("Logic rules list (DB)\n");

		CMySqlRow row;
		while (true == result.GetNextRow(&row))
		{
			uint ruleID = row.GetInt(0);
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
		}
	}
}


CRule * CRuleManager::GetRuleWithID(size_t ruleID)
{
	map <size_t, CRule *>::const_iterator it = m_ruleMap.find(ruleID);
	if (m_ruleMap.end() != it)
	{
		return (it->second);
	}

	return NULL;
}


bool CRuleManager::AddRule(CRule * rule)
{
	size_t ruleID = rule->GetID();
	if (NULL == GetRuleWithID(ruleID))
	{
		m_ruleMap[ruleID] = rule;
		return true;
	}
	else
	{
		DEBUG_ASSERT(false);
	}

	return false;
}


void CRuleManager::OnUpdate()
{
	// FIXME: bruteforce, change to (on_sensor_value_changed update affacted rules)
	map <size_t, CRule *>::const_iterator it = m_ruleMap.begin();

	while (m_ruleMap.end() != it)
	{
		CRule * rule = (it->second);
		
		g_sensorMgr->UpdateSensor(
			rule->GetTargetSensorID(),
			rule->GetValue());

		++it;
	}
}