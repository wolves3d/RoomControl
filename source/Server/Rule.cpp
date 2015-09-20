#include "pch.h"
#include "Server.h"


CRule::CRule(size_t ruleID, size_t targetSensorID)
	: m_isInverted(false)
	, m_ID(ruleID)
	, m_targetSensorID(targetSensorID)
	, m_sensorA(NULL)
	, m_logicOp(NULL)
	, m_sensorB(NULL)
{
}


size_t CRule::GetID() const
{
	return m_ID;
}


float CRule::GetValue() const
{
	return (true == Evaluate())
		? 1.0f
		: 0.0f;
}


void CRule::SetInverted(bool isInverted)
{
	m_isInverted = isInverted;
}


void CRule::SetOperands(ISensor *a, ILogicOp *logicOp, ISensor *b)
{
	m_sensorA = a;
	m_logicOp = logicOp;
	m_sensorB = b;
}


bool CRule::Evaluate() const
{
	bool result = false; // default is (false)

	if (NULL != m_logicOp)
	{
		result = m_logicOp->Evaluate(m_sensorA, m_sensorB);
	}

	return (true == IsInverted())
		? (!result)
		: result;
}