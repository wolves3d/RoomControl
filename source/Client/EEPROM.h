class ReadEEPROM : public INetCommand
{
	COMMAND_HEADER(ReadEEPROM, CMD_READ_EEPROM, RSP_READ_EEPROM);

	virtual uint OnFillData(void * buffer, uint maxByteCount)
	{
		byte * buf = (byte *)buffer;
		buf[0] = 0; // EEPROM offset
		buf[1] = 8; // byte count to read from EEPROM
		return 2;
	}

	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		printf("RSP: EEPROM");

		for (uint i = 0; i < size; ++i)
		{
			printf(" %02X", data[i]);
		}

		printf("\n");
	}
};