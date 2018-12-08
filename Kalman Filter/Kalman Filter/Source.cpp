#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "ArduinoSerial.h"
#include "State.h"

#include <tmmintrin.h> //SSSE3

#include <stdlib.h>
#include <engine.h>

union
{
	__m128 a4;
	float a[4];
} sseF;

std::atomic<bool> bRender = false;
std::atomic<bool> bClearToExit = false;
std::atomic<bool> bStartCapture = false;

void exitState(Serial &A)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRender = false;
			A.~Serial();
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

__m128 velCalc(__m128 accelTable, int delay) // divide all three then sum
{
	__m128 delayVec = _mm_set_ps(delay, delay, delay, delay);
	__m128 velVec = _mm_div_ps(accelTable, delayVec);

	velVec = _mm_hadd_ps(accelTable, accelTable);
	velVec = _mm_hadd_ps(velVec, velVec);

	return velVec;
}

void processingData(State &kp, State &kc, State &kn)
{
	__m128 preVec = _mm_set_ps(0.0f ,kp.m_acx, kp.m_acy, kp.m_acz);
	__m128 kVec = _mm_set_ps(0.0f, kc.m_acx, kc.m_acy, kc.m_acz);

	sseF.a4 = velCalc(kVec, 1000);
	
	float velocity = sseF.a[1];
}

void update(double(&d)[10], double factor) // sample code
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
	double timescale[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	double darr[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double xarr[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double earr[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	if (!(ep = engOpen("\0"))) 
	{
		fprintf(stderr, "\nCan't start MATLAB engine\n");
	}
	
	T = mxCreateDoubleMatrix(1, 10, mxREAL);
	D = mxCreateDoubleMatrix(1, 10, mxREAL);
	E = mxCreateDoubleMatrix(1, 10, mxREAL);
	X = mxCreateDoubleMatrix(1, 10, mxREAL);

	memcpy((void *)mxGetPr(T), (void *)timescale, sizeof(timescale));
	memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
	memcpy((void *)mxGetPr(E), (void *)earr, sizeof(earr));
	memcpy((void *)mxGetPr(X), (void *)xarr, sizeof(xarr));

	engPutVariable(ep, "T", T);
	engPutVariable(ep, "D", D);
	engPutVariable(ep, "E", E);
	engPutVariable(ep, "X", X);

	bRender = true;
	while (bRender) //animating graph example
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
	State kprev(); State K; State kpred();
	
	//Initial MatLAB object construction
	Engine *ep;
	mxArray *T = NULL; mxArray *D = NULL;
	mxArray *X = NULL; mxArray *E = NULL;

	std::thread keyboardListen(exitState, std::ref(*Arduino)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino));
	//std::thread processingThread(processingData);
	std::thread matlabDisplay(matlabPlot, std::ref(ep), std::ref(T), std::ref(D), std::ref(E), std::ref(X));

	keyboardListen.join();
	sensorDataGet.join();
	//processingThread.join();
	matlabDisplay.join();

	return 0;
}