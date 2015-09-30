



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

	virtual	void FillHeader(void * dst, uint cmdID, uint cmdTag, uint byteCount)
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

	virtual uint GetCommandTag(const void * packetBytes)
	{
		return 0;//
	}
};
