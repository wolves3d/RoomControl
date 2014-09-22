#include "pch.h"
#include "serialport.h"


// http://www.codeproject.com/Articles/3061/Creating-a-Serial-communication-on-Win


CSerialPort::CSerialPort()
	: m_port(INVALID_HANDLE_VALUE)
{

}


bool CSerialPort::IsValid()
{
	return (INVALID_HANDLE_VALUE != m_port);
}


bool CSerialPort::Open(const char * portName)
{
	if (NULL != portName)
	{
		m_port = CreateFile(
			portName,							// Specify port device: default "COM1"
			GENERIC_READ | GENERIC_WRITE,		// Specify mode that open device.
			0,                                  // the COM-port can't be shared
			NULL,                               // the object gets a default security.
			OPEN_EXISTING,                      // Specify which action to take on file. 
			0 /*FILE_FLAG_OVERLAPPED*/,				// default.
			NULL);

		if (INVALID_HANDLE_VALUE != m_port)
		{
			// Get the current options for the port...
			if (0 != GetCommState(m_port, &m_config))
			{
				//Define serial connection parameters for the arduino board
				m_config.BaudRate	= CBR_9600;
 				m_config.ByteSize	= 8;
 				m_config.StopBits	= ONESTOPBIT;
 				m_config.Parity		= NOPARITY;
			
				// Set current configuration of serial communication port.
				SetCommState(m_port, &m_config);
				return true;
			}
		}
		else
		{
			LPVOID lpMsgBuf;
			LPVOID lpDisplayBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );

			const char * lpszFunction = "FFF";

			// Display the error message and exit the process

			lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
				(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 

			sprintf_s((LPTSTR)lpDisplayBuf, 
				LocalSize(lpDisplayBuf) / sizeof(TCHAR),
				TEXT("%s failed with error %d: %s"), 
				lpszFunction, dw, lpMsgBuf); 
			MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

			LocalFree(lpMsgBuf);
			LocalFree(lpDisplayBuf);
		}
	}

	return false;
}


void CSerialPort::Close()
{
	if (INVALID_HANDLE_VALUE != m_port)
	{
		if (0 != CloseHandle(m_port))
		{
			// succeed
			return;
		}
		else
		{
			// error closing port!
		}

		m_port = INVALID_HANDLE_VALUE;
	}
	else
	{
		// error invalid operation!
	}
}


uint CSerialPort::Send(const void * data, uint byteCount)
{
	if (INVALID_HANDLE_VALUE != m_port)
	{
		DWORD writtenBytes;
		if (0 != WriteFile(m_port, data, byteCount, &writtenBytes, NULL))
		{
			// succeed
			return writtenBytes;
		}
	}
	else
	{
		// error invalid op!
	}

	return 0;
}


uint CSerialPort::Recv(void * buffer, uint maxByteCount)
{
	if (INVALID_HANDLE_VALUE != m_port)
	{
		DWORD readBytes;
		if (0 != ReadFile(m_port, buffer, maxByteCount, &readBytes, NULL))
		{
			// succeed
			return readBytes;
		}
	}
	else
	{
		// error invalid op!
	}

	return 0;
}
