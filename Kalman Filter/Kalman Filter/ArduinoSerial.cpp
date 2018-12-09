#include "ArduinoSerial.h"

Serial::Serial(const char *portName)
{
	this->connected = false;

	// Connect to port
	this->hSerial = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Validate Connection

		if (this->hSerial == INVALID_HANDLE_VALUE)
		{
			//If not success full display an Error
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
				exit(2);
			}
			else
			{
				printf("ERROR!!!");
				exit(3);
			}
		}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			printf("Failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				this->connected = true;
				PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR); // clear data buffer
			}
		}
	}

}

Serial::~Serial()
{
	if (this->connected)
	{
		this->connected = false;
		CloseHandle(this->hSerial); //Close the serial handler
	}
}

bool Serial::ReadData(State &K)
{
	DWORD bytesRead;
	unsigned int toRead = 0;
	unsigned char buffer[60] = { 0 };

	ClearCommError(this->hSerial, &this->errors, &this->status);

	union 
	{
		float f;
		unsigned char bytes[4];
	} u;

	float sensorReadings[13]; //sensordata

	while (1)
	{
		
		ReadFile(this->hSerial, buffer, 1, &bytesRead, NULL);
		if (buffer[0] == 0x90)
		{
			std::cout << "Start\n";
			int i = 0;
			while (buffer[0] != 0x10)
			{
				ReadFile(this->hSerial, buffer, 4, &bytesRead, NULL);
				toRead += 4;
				if (toRead > 58)
				{
					std::cout << "BAD READ!\n";
					return false;
				}
				if (i < 13)
				{
					u.bytes[0] = buffer[0];
					u.bytes[1] = buffer[1];
					u.bytes[2] = buffer[2];
					u.bytes[3] = buffer[3];
					sensorReadings[i] = u.f;
					i++;
				}
			}
			if (buffer[0] == 0x10)
			{
				bool bBadRead = false;
				if (sensorReadings[11] == 0 && sensorReadings[10] == 0 && sensorReadings[9] == 0 && sensorReadings[8] == 0 && sensorReadings[7] == 0 && sensorReadings[6] == 0)
				{
					std::cout << "INVALID AC/GY READ!\n";
					return false;
				}
				if ((sensorReadings[0] < -180 || sensorReadings[1] < -180) || (sensorReadings[0] > 180 || sensorReadings[1] > 180))
				{
					std::cout << "BAD LAT/LONG READ!\n";
					return false;
				}
				for (int i = 0; i < sizeof(sensorReadings) / sizeof(float); i++)
				{
					if (sensorReadings[i] > 1000000000 || sensorReadings[i] < -1000000000)
					{
						std::cout << "BAD ELEMENT READ!\n";
						return false;
					}
				}
				K.dataSet(sensorReadings);
				//K.m_time = time(0); // set current time
				K.printDataSet();
				std::cout << "End\n";
				return true;
			}
		}
	}
}

bool Serial::WriteData(unsigned char c)
{
	DWORD bytesSent;

	if (!WriteFile(this->hSerial, &c, 1 ,&bytesSent, 0))
	{
		ClearCommError(this->hSerial, &this->errors, &this->status);
		return false;
	}
	else
	{
		return true;
	}
}

bool Serial::IsConnected()
{
	return this->connected;
}