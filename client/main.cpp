#include "pch.h"
#include "CommandManager.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/easy.h>


std::string u_string_format(const char *fmt, ...)
{

	char ret[4 * 4096 + 1] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(ret, 4 * 4096, fmt, ap);
	va_end(ap);

	std::string str(ret);

	return str;
}


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



void OnEnumerationDone()
{
	for (uint i = 0; i < g_commMgr->GetOneWireDeviceCount(); ++i)
	{
		OneWireAddr addr = g_commMgr->GetOneWireDeviceID(i);

		g_commMgr->PushCommand(
			CMD_OW_READ_TEMP_SENSOR_DATA,
			addr.Address(),
			OneWireAddr::ADDR_LEN);
	}
	
}


int main()
{
	CSerialPort comPort;
	if (false == comPort.Open(ARDUINO_PORT))
	{
		printf("Failed to open %s", ARDUINO_PORT);
		return -1;
	}

	printf("Opened %s\n", ARDUINO_PORT);
	usleep(2000 * 1000); // time to init microcontroller
	CommandManager commMgr(&comPort);

//	uint logDelay = 1000 * 60 * 30;
	uint logDelay = 5;
	uint deadline = 0;

	// -------------------------------------------------------------------------

	bool readFlag = false;
	g_commMgr->PushCommand(CMD_REQUEST_ONE_WIRE_ENUM);
	
	while (true)
	{
		commMgr.Update();
		usleep(100 * 1000);

		if (false == readFlag)
		{
			if (true == g_commMgr->IsOneWireEnumerated())
			{
				OnEnumerationDone();
				readFlag = true;

				byte data[] = { 4, 0 };
				g_commMgr->PushCommand(CMD_PIN_WRITE, &data, sizeof(data));
			}
		}

		if (true == readFlag)
		{
			const uint currentTime = time(NULL);
			if (currentTime >= deadline)
			{
				deadline = (currentTime + logDelay);
				OneWireAddr addr = g_commMgr->GetOneWireDeviceID(0);

				g_commMgr->PushCommand(
					CMD_OW_READ_TEMP_SENSOR_DATA,
					addr.Address(),
					OneWireAddr::ADDR_LEN);
			}
		}
	}

	/*
	while (counter < exitBytes)
	{
		const uint r = comPort.Recv(buffer, 256);

		if (r < 256)
		{
			buffer[r] = 0;
			counter += r;

			//if (r > 0)
			{
				printf("%d: %s\n", r, buffer);
			}
		}

		sleep(1);
	}
	*/

	comPort.Close();
	return 0;
}
