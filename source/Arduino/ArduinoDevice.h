#ifndef __ArduinoDevice_h_included__
#define __ArduinoDevice_h_included__


class ArduinoDevice
{
	IAbstractSocket * m_comPort;

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

	ArduinoDevice(IAbstractSocket * socket);

	IAbstractSocket * GetPort() { return m_comPort; }

	bool IsOneWireEnumerated() const { return m_isOneWireEnumerated; }
	uint GetOneWireDeviceCount() { return m_owDeviceList.size(); }
	const OneWireAddr & GetOneWireDevice(uint device);

	void SetUID(const byte *data, uint dataSize);
	const byte * GetUID() const;

	enum
	{
		UID_SIZE = 8,
	};

private:

	byte m_UID[UID_SIZE];
};


#endif // #ifndef __ArduinoDevice_h_included__