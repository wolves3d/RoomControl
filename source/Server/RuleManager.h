

struct ILogicOp
{
	virtual const char * GetName() const = 0;
	virtual bool Evaluate(ISensor *a, ISensor *b) = 0;
};


class CRule : public ISensor
{
public:

	// ISensor
	virtual float GetValue() const;
	virtual size_t GetID() const;

	// CRule
	CRule(size_t ruleID, size_t targetSensorID = 0);

	void SetOperands(ISensor *a, ILogicOp *logicOp, ISensor *b);

	void SetInverted(bool isInverted);
	bool IsInverted() const { return m_isInverted; }

	bool Evaluate() const;
	size_t GetTargetSensorID() const { return m_targetSensorID; }

private:

	size_t m_ID;
	size_t m_targetSensorID;
	ISensor *m_sensorA;
	ILogicOp *m_logicOp;
	ISensor *m_sensorB;

	bool m_isInverted;
};


class CRuleManager
{
public:
	void Init();
	void OnUpdate();

private:
	void InitLogicOps();
	void InitRules();

	void AddLogicOp(ILogicOp *logicOp);
	ILogicOp * GetLogicOpWithName(const char * szName);

	void AddLogicOpWithID(size_t opID, ILogicOp *logicOp);
	ILogicOp * GetLogicOpWithID(size_t opID);

	bool AddRule(CRule * rule);
	CRule * GetRuleWithID(size_t ruleID);

	map <string, ILogicOp *> m_logicOpMapWithName;
	map <size_t, ILogicOp *> m_logicOpMapWithID;

	map <size_t, CRule *> m_ruleMap;
};