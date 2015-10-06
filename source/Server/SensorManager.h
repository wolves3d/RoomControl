// =============================================================================
#ifndef __SensorManager_h_included__
#define __SensorManager_h_included__
// =============================================================================


// forward declaration
class CMySqlClient;


struct ISensor
{
	virtual ~ISensor() {}
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
	const char * GetPath() const { return m_path.c_str(); }

	virtual bool IsArduinoSensor() const { return false; }

private:
	size_t m_id;
	string m_path;
	float m_value;

	void SetValue(float newVal) { m_value = newVal; }
};


class CArduinoSensor : public CSensor
{
public:

	// ÑSensor
	virtual bool IsArduinoSensor() const { return true; }

	CArduinoSensor(uint id, const string & path, const float * optValue = NULL);
	static bool IsArduinoSensor(const char *sensorPath);
	const string & GetPinName() const { return m_pinName; }
	const string & GetArduinoUID() const { return m_arduinoUID; }

private:
	string m_arduinoUID;
	string m_pinName;
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

public:

	CSensorManager();
	bool Init(CMySqlClient * dbConn);
	void UpdateSystemSensors();
	bool UpdateSensor(uint id, float newValue, bool isSetter, bool forceUpdateClient = false);

	uint GetSensorIdByPath(const char * szSensorPath);
	CSensor * GetSensor(uint id);
};


extern CSensorManager * g_sensorMgr;
	
// =============================================================================
#endif // #ifndef __SensorManager_h_included__
// =============================================================================