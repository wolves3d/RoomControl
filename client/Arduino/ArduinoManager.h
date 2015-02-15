#include "../arduino/src/netCommand.h"
#include "../arduino/src/OneWireWrapper.h"


class ArduinoDevice
{
	// OneWire -----------------------------------------------------------------
	friend class OneWireEnumBegin;
	friend class OneWireRomFound;
	friend class OneWireEnumEnd;

	vector <OneWireAddr> m_owDeviceList;
	bool m_isOneWireEnumerated;

	void ClearOneWireDeviceList();
	void AddOneWireDevice(OneWireAddr deviceAddr);
	void OnOneWireEnumerated();

public:

	//ArduinoDevice();

	bool IsOneWireEnumerated() const { return m_isOneWireEnumerated; }
	uint GetOneWireDeviceCount() { return m_owDeviceList.size(); }
	const OneWireAddr & GetOneWireDevice(uint device);
};


class ArduinoManager
{
	typedef map <IAbstractSocket *, ArduinoDevice *> DeviceMap;
	DeviceMap m_deviceMap;

public:

	ArduinoManager();
	ArduinoDevice * FindDevice(IAbstractSocket * socket, bool failOnNotFound = false);
};


extern ArduinoManager * g_arduinoManager;