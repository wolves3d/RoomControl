class CSerialPort
{
public:
	bool Open(const char * port);
	void Close();
	uint Send(void * data, uint byteCount);
	uint Recv(void * buffer, uint maxByteCount);
private:

	int m_file;
};