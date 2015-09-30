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


class CGreaterOperator : public ILogicOp
{
	virtual const char * GetName() const { return "greater"; }

	virtual bool Evaluate(ISensor *a, ISensor *b)
	{
		if ((NULL != a) && (NULL != b))
		{
			const float operandA = a->GetValue();
			const float operandB = b->GetValue();
			return (operandA > operandB);
		}
		

		DEBUG_ASSERT(NULL != a);
		DEBUG_ASSERT(NULL != b);
		return false;
	}
};


class CGreaterEqualOperator : public ILogicOp
{
	virtual const char * GetName() const { return "greater_or_equal"; }

	virtual bool Evaluate(ISensor *a, ISensor *b)
	{
		if ((NULL != a) && (NULL != b))
		{
			const float operandA = a->GetValue();
			const float operandB = b->GetValue();
			return (operandA >= operandB);
		}


		DEBUG_ASSERT(NULL != a);
		DEBUG_ASSERT(NULL != b);
		return false;
	}
};


void CRuleManager::InitLogicOps()
{
	AddLogicOp(new COddOperator);
	AddLogicOp(new CGreaterOperator);
	AddLogicOp(new CGreaterEqualOperator);


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

			

			ILogicOp * logicOp = GetLogicOpWithName(opertaorName.c_str());
			if (NULL != logicOp)
			{
				printf("id %d name %s\n", opertaorID, opertaorName.c_str());
				AddLogicOpWithID(opertaorID, logicOp);
			}
			else
			{
				printf("id %d name %s (NO HANDLER)\n", opertaorID, opertaorName.c_str());
			}
		}
	}
}