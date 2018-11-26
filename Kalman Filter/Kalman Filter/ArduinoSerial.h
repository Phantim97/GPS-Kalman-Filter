#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 500

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

class Serial
{
private:
	HANDLE hSerial; // Serial comm handler
	bool connected; // Connection status
	COMSTAT status; // Status of serial port
	DWORD errors;   // Keep track of last error

public:
	Serial(const char *portName);
	~Serial(); // close and clean up connection
	int ReadData(char *buffer, unsigned int nbChar);
	int ReadDataTest(char *buffer, unsigned int nbChar);
	bool IsConnected();
};

#endif // SERIALCLASS_H_INCLUDED