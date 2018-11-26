#include <iostream>
#include <vector>
#include <thread>
#include "ArduinoSerial.h"
#include "State.h"
#include <xmmintrin.h>
#include <immintrin.h>
#include <string>

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

void convertToFloat(std::string &buff)
{
	std::vector<std::string> strVec;
	std::vector<float> floatVec;
	std::string temp;

	for (int i = 0; i < buff.size(); i++)
	{
		if (buff[i] == 32 || buff[i] == '\n')
		{
			strVec.push_back(temp);
			temp = "";
		}
		else
		{
			temp += buff[i];
		}
	}

	//Parse out 6th element for time
	std::string time = strVec[5];
	strVec.erase(strVec.begin() + 5); // erase time out of vector

	for (std::vector<std::string>::iterator it = strVec.begin(); it != strVec.end(); it++) //convert strings to floats
	{
		try
		{
			floatVec.push_back(stof(*it));
		}
		catch (const std::exception &e)
		{
			std::cout << "Buffer: ";
			std::cout << e.what() << '\n';
			system("pause");
		}
	}

	std::cout << time << '\n';
	std::cout << "Float Vector:\n";

	for (std::vector<float>::iterator it = floatVec.begin(); it != floatVec.end(); it++) //debugging (verify successful convert)
	{
		std::cout << *it << '\n';
	}

}

void sensorData(Serial &A)
{

	if (A.IsConnected())
	{
		std::cout << "Connection Established\n";
	}

	char incomingData[256] = "";
	int dataLength = 79;
	int readResult = 0;
	int count = 0; // for debugging
	int bad_packets = 0;
	double avg_loss = 0.0;
	int current_packet = 0;
	while (A.IsConnected())
	{
		readResult = A.ReadData(incomingData, dataLength);
		incomingData[readResult] = 0;
		if (readResult < 75 || readResult > 95) // Not complete buffer
		{
			std::cout << "Bad Packet: " << readResult << '\n'; //Shows bad packet and size of packet
			bad_packets++;
		}
		else
		{
			std::string dat = incomingData;
			convertToFloat(dat);
		}
		Sleep(1000);
		count++; // for debugging
		if (count == 50) // Get average data loss of 50 iterations
		{
			avg_loss = (avg_loss + (bad_packets / 50.0)) / 2;
			system("cls");
			bad_packets = 0;
			count = 0;
			std::cout << "Average Loss = " << avg_loss << '\n';
		}
		std::fill_n(incomingData,dataLength, ' '); //empty buffer on completion
	}
}

void processingData(State &kp, State &kc, State &kn)
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
	State kprev(), kcurrent(), kpred();

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