#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 1000

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

class Serial
{
private:
	HANDLE hSerial; //Serial comm handler
	bool connected; //Connection status
	COMSTAT status; // status of serial port
	DWORD errors; //Keep track of last error

public:
	Serial(const char *portName);
	~Serial(); // close and clean up connection
	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	int ReadData(char *buffer, unsigned int nbChar);
	bool IsConnected();
};

#endif // SERIALCLASS_H_INCLUDED