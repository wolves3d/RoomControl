#include "pch.h"
#include "serialport.h"

#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

bool CSerialPort::Open(const char * port)
{
	if (NULL != port)
	{
		m_file = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

		if (-1 != m_file)
		{
			fcntl(m_file, F_SETFL, FNDELAY);

			// Get the current options for the port...
			struct termios options;
			tcgetattr(m_file, &options);

			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);

			//  Enable the receiver and set local mode...
			options.c_cflag |= (CLOCAL | CREAD);

			// Set the new options for the port...
			tcsetattr(m_file, TCSANOW, &options);

			return true;
		}
	}

	return false;
}


void CSerialPort::Close()
{
	if (NULL != m_file)
	{
		close(m_file);
		m_file = NULL;
	}
}


uint CSerialPort::Send(void * data, uint byteCount)
{
	return 0;
}


uint CSerialPort::Recv(void * buffer, uint maxByteCount)
{
	if (NULL != m_file)
	{
		return read(m_file, buffer, maxByteCount);
	}

	return 0;
}
