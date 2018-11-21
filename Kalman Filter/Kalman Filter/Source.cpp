#include <iostream>
#include <vector>
#include <thread>
#include "ArduinoSerial.h"
#include <xmmintrin.h>

void exitState(Serial &A)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			A.~Serial();
			exit(1);
		}
	}
}

void sensorData(Serial &A)
{

	if (A.IsConnected())
		std::cout << "Connection Established\n";

	char incomingData[256] = "";
	int dataLength = 255;
	int readResult = 0;

	while (A.IsConnected())
	{
		readResult = A.ReadData(incomingData, dataLength);
		incomingData[readResult] = 0;
		std::cout << incomingData;
		Sleep(500);
	}
}

void processingData()
{
	//Apply Filter to data
}

void matlabPlot()
{
	// Render Matlab Graphs
}

int main() //Primary Driver
{
	Serial *Arduino = new Serial("COM3");

	std::thread keyboardListen(exitState, std::ref(*Arduino)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino));
	//std::thread processingThread(processingData);
	//std::thread matlabDisplay(matlabPlot);

	keyboardListen.join();
	sensorDataGet.join();
	//processingThread.join();
	//matlabDisplay.join();

	return 0;
}