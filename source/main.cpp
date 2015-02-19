#include "pch.h"
//#include "CommandManagerArduino.h"

#include "CommandManager.h"
#include "serialport.h"
#include "tcp_socket.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/easy.h>

#include "Arduino/ArduinoManager.h"
#include "Arduino/CommandHandlers.h"

#include "Server/Server.h"


void LogDB(const char * sensorID, float sensorT)
{
	const string serverURL = "192.168.0.1/probe_data.php";

	string postBody = u_string_format("probe_uid=%s&probe_data=%f",
		sensorID, sensorT);

	// cURL init -----------------------------------------------------------
	CURL * curl = curl_easy_init();

	if (NULL == curl)
		return;

	curl_easy_setopt(curl, CURLOPT_URL, serverURL.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, m_timeOutSec);

	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

// 	if (true == m_targetFileName.empty()) // Where do we store result?
// 	{
// 		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FlushStringBuffer);
// 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_resultBody);
// 	}
// 	else
// 	{
// 		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FlushFileBuffer);
// 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
// 
// 		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
// 		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DownloadProgress);
// 		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
// 	}
// 
// 	if (0 != m_offset)
// 		curl_easy_setopt(curl, CURLOPT_RESUME_FROM, m_offset);

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postBody.length());

	// cURL perform --------------------------------------------------------

	CURLcode result = curl_easy_perform(curl);

	bool isSucceed = false;
	int requestStatusCode;
	CURLcode getResponseCode = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &requestStatusCode);

// 	CCLOG("DownloadTask: responseCode %d (url: %s)", getResponseCode, m_assetURL.c_str());
// 	CCLOG("DownloadTask: requestStatusCode %d", requestStatusCode);

	if (CURLE_OK == getResponseCode)
	{
		if (CURLE_OK == result)
		{
			// Curl can't handle "disk is full" case
			//if (false == m_writeErrorFlag)
			{
				isSucceed = ((200 == requestStatusCode) ||
					(206 == requestStatusCode) || // partially download success
					(416 == requestStatusCode));  // already downloaded
			}
		}
		else if (CURLE_RANGE_ERROR == result)
		{
			// Hack to allow 100% download
			isSucceed = (416 == requestStatusCode);
		}
	}

	// finally -------------------------------------------------------------
	curl_easy_cleanup(curl);
}


/*
void OnEnumerationDone()
{
	for (uint i = 0; i < g_commMgr->GetOneWireDeviceCount(); ++i)
	{
		OneWireAddr addr = g_commMgr->GetOneWireDeviceID(i);

		g_commMgr->SendCommand(
			CMD_OW_READ_TEMP_SENSOR_DATA,
			addr.Address(),
			OneWireAddr::ADDR_LEN);
	}
}
*/

class ClientGetGUIDResponse : public IResponseHandler
{
	HANDLER_HEADER(ClientGetGUIDResponse, CMD_GET_CLIENT_GUID)
	{
		printf("client: response - get client GUID\n");
		mgr->SendCommand(socket, RSP_GET_CLIENT_GUID, NULL, 0);
	}
};


int main()
{
	CServer server(SERVER_PORT);

	// arduino -----------------------------------------------------------------

	ArduinoManager arduinoMgr;
	
	CSerialPacket packet;
	CCommandManager arduinoCmdManager(&packet);
	// FIXME: memory leak, no free on destruct
	arduinoCmdManager.RegisterHandler(new OneWireEnumBegin());
	arduinoCmdManager.RegisterHandler(new OneWireRomFound());
	arduinoCmdManager.RegisterHandler(new OneWireEnumEnd());
	arduinoCmdManager.RegisterHandler(new OneWireTemperature());
	arduinoCmdManager.RegisterHandler(new PingResponse());
	arduinoCmdManager.RegisterHandler(new ReadEEPROM());
	arduinoCmdManager.RegisterHandler(new WriteEEPROM());

	CSerialPort arduinoPort;
	if (true == arduinoPort.Open(ARDUINO_PORT))
	{
		System::SleepMS(2000); // time to init microcontroller
		arduinoCmdManager.GetPacketManager()->AddClent(&arduinoPort);
	}

	ArduinoDevice * device = arduinoMgr.FindDevice(&arduinoPort); // register device


//	uint logDelay = 1000 * 60 * 30;
	uint logDelay = 5;
	time_t deadline = 0;
	
	// -------------------------------------------------------------------------

	//CServer server(SERVER_PORT);

	// -------------------------------------------------------------------------

	CCommandManager netCmdManager(new CNetPacket());
	netCmdManager.RegisterHandler(new ClientGetGUIDResponse());

	CTcpSocket serverSocket;
	serverSocket.Connect("127.0.0.1", SERVER_PORT);
	netCmdManager.GetPacketManager()->AddClent(&serverSocket);

	// -------------------------------------------------------------------------

	bool readFlag = false;

	/*
	byte args[] = { 0, 8, 0, 1, 2, 3, 4, 5, 6, 7 };
	g_commMgr->PushCommand(CMD_WRITE_EEPROM, &args, sizeof(args));
	*/

	
// 	byte args[] = { 0, 8 };
// 	arduinoCmdManager.SendCommand(&arduinoPort, CMD_READ_EEPROM, &args, sizeof(args));
	

	arduinoCmdManager.SendCommand(&arduinoPort, CMD_REQUEST_ONE_WIRE_ENUM, NULL, 0);
	
	while (true)
	{
		server.OnUpdate();
		netCmdManager.OnUpdate();
		arduinoCmdManager.OnUpdate();
		System::SleepMS(100);

		if (false == readFlag)
		{
			if (true == device->IsOneWireEnumerated())
			{
				//OnEnumerationDone();
				readFlag = true;

				// test "turn on relay"
				byte data[] = { 4, 0 };
				arduinoCmdManager.SendCommand(&arduinoPort, CMD_PIN_WRITE, &data, sizeof(data));
			}
		}

		if (true == readFlag)
		{
			const time_t currentTime = time(NULL);
			if (currentTime >= deadline)
			{
				deadline = (currentTime + logDelay);
				OneWireAddr addr = device->GetOneWireDevice(0);

				arduinoCmdManager.SendCommand(
					&arduinoPort,
					CMD_OW_READ_TEMP_SENSOR_DATA,
					addr.Address(),
					OneWireAddr::ADDR_LEN);
			}
		}
	}

//	comPort.Close();
	return 0;
}
