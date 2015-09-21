// =============================================================================
#ifndef __SensorManager_h_included__
#define __SensorManager_h_included__
// =============================================================================


// forward declaration
class CMySqlClient;


struct ISensor
{
	virtual float GetValue() const = 0;
	virtual size_t GetID() const = 0;
};


class CSensor : public ISensor
{
friend class CSensorManager;

public:

	// ISensor
	virtual float GetValue() const { return m_value; }
	virtual size_t GetID() const { return m_id; }

	CSensor(uint id, const string & path, const float * optValue = NULL);

	void UpdateValue(float newValue);
	const char * GetPath() const { return m_path.c_str(); }

private:
	size_t m_id;
	string m_path;
	float m_value;

	void SetValue(float newVal) { m_value = newVal; }
};


class CSensorManager
{
	CMySqlClient * m_dataBase;

	typedef map <uint, CSensor *> SensorMap;
	SensorMap m_sensorMap;

	uint m_secSensorID;
	uint m_minuteSensorID;
	uint m_hourSensorID;
	uint m_daySensorID;
	uint m_weekDaySensorID;
	uint m_monthSensorID;
	uint m_yearSensorID;

	uint GetSensorIdByPath(const char * szSensorPath);

public:

	CSensorManager();
	bool Init(CMySqlClient * dbConn);
	void UpdateSystemSensors();
	bool UpdateSensor(uint id, float newValue);

	CSensor * GetSensor(uint id);
};


extern CSensorManager * g_sensorMgr;
	
// =============================================================================
#endif // #ifndef __SensorManager_h_included__
// =============================================================================