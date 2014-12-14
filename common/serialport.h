class CSerialPort
{
public:
	CSerialPort();
	bool Open(const char * portName);
	bool IsValid();
	void Close();
	uint Send(const void * data, uint byteCount);
	uint Recv(void * buffer, uint maxByteCount);

private:

#ifdef WIN32
	HANDLE	m_port;
	DCB		m_config;
#else
	int m_file;
#endif

};