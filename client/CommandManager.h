#include "../arduino/source/netCommand.h"
#include "../arduino/source/OneWireWrapper.h"
#include "serialport.h"

// Forward declarations
class CSerialPort;


class CommandManager
{
	struct Command
	{
		// Mac packet size 16 byte
		byte cmd;
		byte byteCount;
		//byte crc;

		byte args[14];

		uint GetSize() const
		{
			return (2 + byteCount);
		}
	};

	vector <Command> m_commandList;
	CSerialPort m_port;
	string m_portName;
	bool m_waitForAnswerMode;

	vector <OneWireAddr> m_owDeviceList;
	bool m_isOneWireEnumerated;

	bool CheckPort();
	bool TrySendCommand();
	bool GetMoreData(void * buffer, uint byteCount);

public:
	CommandManager(const char * portName);
	void PushCommand(ECommandID cmd, void * data = NULL, uint byteCount = 0);
	void Update();

	bool IsOneWireEnumerated() const { return m_isOneWireEnumerated; }
	uint GetOneWireDeviceCount();
	const OneWireAddr & GetOneWireDeviceID(uint device);
};


extern CommandManager * g_commMgr;