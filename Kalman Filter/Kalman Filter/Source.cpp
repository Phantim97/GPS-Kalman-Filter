#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "ArduinoSerial.h"
#include "State.h"

#include <xmmintrin.h>
#include <immintrin.h>

#include <stdlib.h>
#include <engine.h>

std::atomic<bool> bRender = false;
std::atomic<bool> bClearToExit = false;
std::atomic<bool> bStartCapture = false;

#define BUFSIZE 256

void exitState(Serial &A, State &K)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRender = false;
			A.~Serial();
			delete &K;
			// deconstruct matlab processes;
			while (bRender == false)
			{
				if (bClearToExit == true)
				{
					exit(1);
				}
			}
		}
	}
}

void sensorData(Serial &A, State &K)
{
	if (A.IsConnected())
	{
		std::cout << "Connection Established\n";
	}
	Sleep(2000); //remove initial bad reads
	while (A.IsConnected())
	{
		A.ReadData(K);
		Sleep(10);
	}
}

void processingData(State &kp, State &kc, State &kn)
{
	//Apply Filter to data
}

void update(double(&d)[3])
{
	srand(time(0));
	for (int i = 0; i < sizeof(d) / sizeof(double); i++)
	{
		d[i] = rand() % 100;
	}
}

void matlabPlot(Engine *ep, mxArray *T, mxArray *D)
{
	// Render Matlab Graph
	char buffer[BUFSIZE];
	double timescale[3] = { 0.0, 1.0, 2.0 };
	double darr[3] = { 0,0,0 };
	if (!(ep = engOpen("\0"))) 
	{
		fprintf(stderr, "\nCan't start MATLAB engine\n");
	}

	T = mxCreateDoubleMatrix(1, 3, mxREAL);
	D = mxCreateDoubleMatrix(1, 3, mxREAL);
	memcpy((void *)mxGetPr(T), (void *)timescale, sizeof(timescale));
	memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
	engPutVariable(ep, "T", T);
	engPutVariable(ep, "D", D);
	bRender = true;
	while (bRender)
	{
		update(darr);
		memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
		engPutVariable(ep, "D", D);
		engEvalString(ep, "plot(T,D);");
		Sleep(1000);
	}		
	mxDestroyArray(T);
	mxDestroyArray(D);
	engEvalString(ep, "close;");
	engClose(ep);
	bClearToExit = true;
}

int main() //Primary Driver
{
	Serial *Arduino = new Serial("COM3");
	State kprev(); State kpred();
	State *K = new State();

	Engine *ep;
	mxArray *T = NULL;
	mxArray *D = NULL;

	std::thread keyboardListen(exitState, std::ref(*Arduino), std::ref(*K)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino), std::ref(*K));
	//std::thread processingThread(processingData);
	std::thread matlabDisplay(matlabPlot, std::ref(ep), std::ref(T), std::ref(D));

	keyboardListen.join();
	sensorDataGet.join();
	//processingThread.join();
	matlabDisplay.join();

	return 0;
}