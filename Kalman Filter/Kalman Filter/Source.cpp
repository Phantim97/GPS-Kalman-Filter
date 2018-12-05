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

void update(double(&d)[3], double factor)
{
	srand(time(0));
	for (int i = 0; i < sizeof(d) / sizeof(double); i++)
	{
		d[i] = (rand() % 100) * factor;
	}
}

void matlabPlot(Engine *ep, mxArray *T, mxArray *D, mxArray *E, mxArray *X)
{
	// Render Matlab Graph
	char buffer[BUFSIZE];
	double timescale[3] = { 0.0, 1.0, 2.0 };
	double darr[3] = { 0,0,0 };
	double xarr[3] = { 0,0,0 };
	double earr[3] = { 0,0,0 };
	if (!(ep = engOpen("\0"))) 
	{
		fprintf(stderr, "\nCan't start MATLAB engine\n");
	}

	T = mxCreateDoubleMatrix(1, 3, mxREAL);
	D = mxCreateDoubleMatrix(1, 3, mxREAL);
	E = mxCreateDoubleMatrix(1, 3, mxREAL);
	X = mxCreateDoubleMatrix(1, 3, mxREAL);
	memcpy((void *)mxGetPr(T), (void *)timescale, sizeof(timescale));
	memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
	memcpy((void *)mxGetPr(E), (void *)earr, sizeof(earr));
	memcpy((void *)mxGetPr(X), (void *)xarr, sizeof(xarr));

	engPutVariable(ep, "T", T);
	engPutVariable(ep, "D", D);
	engPutVariable(ep, "E", E);
	engPutVariable(ep, "X", X);
	bRender = true;
	while (bRender)
	{
		update(darr, 1.0);
		update(earr, .75);
		update(xarr, .5);
		memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
		memcpy((void *)mxGetPr(X), (void *)xarr, sizeof(xarr));
		memcpy((void *)mxGetPr(E), (void *)earr, sizeof(earr));
		engPutVariable(ep, "D", D);
		engPutVariable(ep, "E", E);
		engPutVariable(ep, "X", X);
		engEvalString(ep, "plot(T, D, '-o', T, E, '-o', T, X, '-o');");
		Sleep(1000);
	}		
	mxDestroyArray(T);
	mxDestroyArray(D);
	mxDestroyArray(X);
	mxDestroyArray(E);
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
	mxArray *X = NULL;
	mxArray *E = NULL;

	std::thread keyboardListen(exitState, std::ref(*Arduino), std::ref(*K)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino), std::ref(*K));
	//std::thread processingThread(processingData);
	std::thread matlabDisplay(matlabPlot, std::ref(ep), std::ref(T), std::ref(D), std::ref(E), std::ref(X));

	keyboardListen.join();
	sensorDataGet.join();
	//processingThread.join();
	matlabDisplay.join();

	return 0;
}