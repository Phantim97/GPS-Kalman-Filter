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

void matlabPlot(Engine *ep, mxArray *T)
{
	// Render Matlab Graph
	mxArray *result = NULL;
	char buffer[BUFSIZE];
	double time[11] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
	if (!(ep = engOpen("\0"))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
	}

	T = mxCreateDoubleMatrix(1, 11, mxREAL);
	memcpy((void *)mxGetPr(T), (void *)time, sizeof(time));
	bRender = true;
	while (bRender)
	{
		engPutVariable(ep, "T", T);
		engEvalString(ep, "D = .5.*(-9.8).*T.^2;");

		engEvalString(ep, "title('Kalman Filter');");
		engEvalString(ep, "xlabel('Yeets');");
		engEvalString(ep, "ylabel('Yotes');");
		engEvalString(ep, "plot(T,D);");
		Sleep(500);
		engPutVariable(ep, "T", T);
		engEvalString(ep, "D = .5.*(-9.8).*T.^3;");
		engEvalString(ep, "plot(T,D)");
		Sleep(500);
	}			
	mxDestroyArray(T);
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

	std::thread keyboardListen(exitState, std::ref(*Arduino), std::ref(*K)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino), std::ref(*K));
	//std::thread processingThread(processingData);
	std::thread matlabDisplay(matlabPlot, std::ref(ep), std::ref(T));

	keyboardListen.join();
	sensorDataGet.join();
	//processingThread.join();
	matlabDisplay.join();

	return 0;
}