#include "pch.h"
#include "client.h"


void OnSetPinValue::OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
{
	DEBUG_ASSERT(3 == size);

	if (3 != size)
		return;

	// FIXME: HACK
	ArduinoVector deviceList = g_arduinoManager->BuildDeviceList();
	ArduinoDevice * dev = deviceList[0];

	
	ArduinoSetPinValue pinSetCommand(data[0], data[1], data[2]);

	g_arduinoCmdManager->SendCommand(
		dev->GetPort(),
		&pinSetCommand);
}