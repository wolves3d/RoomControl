#ifndef __NetProtocol_h_included__
#define __NetProtocol_h_included__


enum ENetCommandID
{
	CMD_GET_CLIENT_GUID = 1,
	RSP_GET_CLIENT_GUID = 2,

	CMD_SET_CLIENT_GUID = 3,
	RSP_SET_CLIENT_GUID = 4,

	// Client to server
	SEND_CLIENT_INFO = 5,
	UPDATE_SENSOR_VALUE = 6,

	// Server to client
	SET_PIN_VALUE = 7,


	NET_TABLE_SIZE	// must be at end of enum
};


#endif // #ifndef __NetProtocol_h_included__