#include "pch.h"
#include "serialport.h"

#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


CSerialPort::CSerialPort()
	: m_file(-1)
{

}


bool CSerialPort::IsValid()
{
	return (-1 != m_file);
}


bool CSerialPort::Open(const char * port)
{
	if (false == IsValid())
	{
		m_file = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

		if (-1 != m_file)
		{
			fcntl(m_file, F_SETFL, FNDELAY);

			// Get the current options for the port...
			struct termios toptions;
			tcgetattr(m_file, &toptions);

/*
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);

			//  Enable the receiver and set local mode...
			options.c_cflag |= (CLOCAL | CREAD);
*/

/* Set custom options */
 
/* 9600 baud */
 cfsetispeed(&toptions, B9600);
 cfsetospeed(&toptions, B9600);

 toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;

			// Set the new options for the port...
			tcsetattr(m_file, TCSANOW, &toptions);

			return true;
		}
	}

	return false;
}


void CSerialPort::Close()
{
	if (true == IsValid())
	{
		close(m_file);
		m_file = -1;
	}
}


uint CSerialPort::Send(const void * data, uint byteCount)
{
	if (true == IsValid())
	{
		return write(m_file, data, byteCount);
	}

	return 0;
}


uint CSerialPort::Recv(void * buffer, uint maxByteCount)
{
	if (true == IsValid())
	{
		int res = read(m_file, buffer, maxByteCount);

		if (res > 0)
		{
			return res;
		}
	}

	return 0;
}
