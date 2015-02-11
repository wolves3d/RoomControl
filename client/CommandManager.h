#include "../arduino/src/netCommand.h"
#include "../arduino/src/OneWireWrapper.h"
#include "serialport.h"

// Forward declarations
class CSerialPort;


class Command
{
public:
	// begin header
		//Max packet size 16 byte
		byte cmd;
		byte byteCount;
		//byte crc;
		byte args[14];
	// end header

public:
	uint GetSize() const
	{
		return (2 + byteCount);
	}
};


class CommandHandler
{
private:
	ECommandID m_cmdID;

public:
	CommandHandler(ECommandID cmdID) : m_cmdID(cmdID) {}
	ECommandID GetCommandID() const { return m_cmdID; }
	virtual void OnResponse(byte * data, uint size) = 0;
};


class CommandManager
{
	CommandHandler * m_handlerTable[ECommandID::CMD_TABLE_SIZE];

	CSerialPort m_port;
	string m_portName;
	bool m_waitForAnswerMode;
	vector <Command> m_commandList;

	vector <OneWireAddr> m_owDeviceList;
	bool m_isOneWireEnumerated;

	// OneWire -----------------------------------------------------------------
	friend class OneWireEnumBegin;
	friend class OneWireRomFound;
	friend class OneWireEnumEnd;
	void ClearOneWireDeviceList();
	void AddOneWireDevice(OneWireAddr deviceAddr);
	void OnOneWireEnumerated();
	// -------------------------------------------------------------------------

	bool CheckPort();
	bool TrySendCommand();
	bool GetMoreData(void * buffer, uint byteCount);

	bool CheckHandlerID(ECommandID cmdID) { return ((cmdID >= 0) && cmdID < (CMD_TABLE_SIZE)); }
	CommandHandler * GetHandlerWithID(ECommandID cmdID);
	void RegisterHandler(CommandHandler * handler);

public:
	CommandManager(const char * portName);
	void PushCommand(ECommandID cmd, void * data = NULL, uint byteCount = 0);
	void Update();

	bool IsOneWireEnumerated() const { return m_isOneWireEnumerated; }
	uint GetOneWireDeviceCount();
	const OneWireAddr & GetOneWireDeviceID(uint device);
};


extern CommandManager * g_commMgr;