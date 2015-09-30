#include "../../arduino/src/netCommand.h"
#include "../../arduino/src/OneWireWrapper.h"

#include "ArduinoDevice.h"


typedef vector <ArduinoDevice *> ArduinoVector;





class ArduinoManager
{
	typedef map <IAbstractSocket *, ArduinoDevice *> DeviceMap;
	DeviceMap m_deviceMap;

public:

	ArduinoManager();
	ArduinoDevice * FindDevice(IAbstractSocket * socket, bool failOnNotFound = false);
	ArduinoVector BuildDeviceList();
};


extern ArduinoManager * g_arduinoManager;