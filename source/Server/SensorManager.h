// =============================================================================
#ifndef __SensorManager_h_included__
#define __SensorManager_h_included__
// =============================================================================


// forward declaration
class CMySqlClient;


class CSensor
{
	friend class CSensorManager;
	uint m_id;
	string m_path;
	float m_value;

	void SetValue(float newVal) { m_value = newVal; }

public:

	CSensor(uint id, const string & path, const float * optValue = NULL);

	uint GetID() const { return m_id; }
	float GetValue() const { return m_value; }
	void UpdateValue(float newValue);
};


class CSensorManager
{
	CMySqlClient * m_dataBase;

	typedef map <uint, CSensor *> SensorMap;
	SensorMap m_sensorMap;

	CSensor * m_minuteSensor;

	CSensor * GetSensor(uint id);

public:

	CSensorManager();
	bool Init(CMySqlClient * dbConn);
	void UpdateSystemSensors();
	bool UpdateSensor(uint id, float newValue);
};


extern CSensorManager * g_sensorMgr;
	
// =============================================================================
#endif // #ifndef __SensorManager_h_included__
// =============================================================================