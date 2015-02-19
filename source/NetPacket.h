

enum ENetCommandID
{
	CMD_GET_CLIENT_GUID = 1,
	RSP_GET_CLIENT_GUID = 2,

	CMD_SET_CLIENT_GUID = 3,
	RSP_SET_CLIENT_GUID = 4,

	NET_TABLE_SIZE	// must be at end of enum
};


class CNetPacket : public ITransportPacket
{
	struct PacketHeader
	{
		int		cmdID;			// Уникальный идентификатор команды
		int		dataByteCount;	// Размер аргумента команды
	};

public:

	// ITransportPacket --------------------------------------------------------

	virtual uint GetHeaderSize() const
	{
		return sizeof(PacketHeader);
	}

	virtual bool CheckHeader(const void * header, uint byteCount)
	{
		return false;
	}

	virtual uint GetArgumentSize(const void * packetBytes)
	{
		return (uint)((PacketHeader *)packetBytes)->dataByteCount;
	}

	virtual bool CheckPacket(const void * packet, uint byteCount)
	{
		return false;
	}

	virtual void FillHeader(void * dst, uint cmdID, uint byteCount)
	{
		if (NULL == dst)
			return;

		PacketHeader * header = (PacketHeader *)dst;

		header->cmdID = cmdID;
		header->dataByteCount = byteCount;

		// FIXME: add CRC checksums here!
	}

	virtual uint GetCommandID(const void * packetBytes)
	{
		return ((PacketHeader *)packetBytes)->cmdID;
	}
};
