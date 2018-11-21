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
		}
		else
		{
			printf("ERROR!!!");
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
			dcbSerialParams.BaudRate = CBR_9600;
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
				Sleep(ARDUINO_WAIT_TIME);
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

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
	DWORD bytesRead;
	unsigned int toRead;

	ClearCommError(this->hSerial, &this->errors, &this->status); // derive status on board

	//Check if there is something to read
	if (this->status.cbInQue > 0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue > nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return 0
	return 0;

}

bool Serial::IsConnected()
{
	return this->connected;
}