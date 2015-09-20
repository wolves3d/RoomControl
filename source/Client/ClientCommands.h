class SendClientInfo : public INetCommand
{
	// Send arduino UID connected to client
	// data size 8 bytes

	COMMAND_HEADER(SendClientInfo, SEND_CLIENT_INFO, CMD_NOP);
	virtual uint OnFillData(void * buffer, uint maxByteCount);
};