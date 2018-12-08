#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "State.h"

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
	bool ReadData(State &k);
	bool WriteData(unsigned char c);
	bool IsConnected();
};

#endif // SERIALCLASS_H_INCLUDED