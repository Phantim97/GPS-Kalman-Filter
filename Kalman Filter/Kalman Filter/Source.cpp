#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "ArduinoSerial.h"
#include "Kalman.h"

#include <tmmintrin.h> //SSSE3

#include <stdlib.h>
#include <engine.h>

std::mutex mu;

union
{
	__m128 a4;
	float a[4];
} sseF;

std::atomic<bool> bRender = false;
std::atomic<bool> bClearToExit = false;
std::atomic<bool> bStartCapture = false;
std::atomic<bool> bGraphed = false;
std::atomic<bool> bEngineLoaded = false;

void exitState(Serial &A, State &Kp, State &Kc)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRender = false;
			A.~Serial();
			// deconstruct matlab processes
			while (bRender == false)
			{
				if (bClearToExit == true)
				{
					// TODO: Handle deallocation of states
					exit(1);
				}
			}
		}
	}
}

inline __m128 velCalc(__m128 accelPrev, __m128 accelCurr, float deltaT)
{
	__m128 deltaVec = _mm_set_ps(deltaT, deltaT, deltaT, deltaT);
	__m128 velVec = _mm_add_ps(accelPrev, accelCurr);

	velVec = _mm_mul_ps(velVec, deltaVec);
	velVec = _mm_hadd_ps(velVec, velVec);
	velVec = _mm_hadd_ps(velVec, velVec);
	return velVec;
}

void processingData(State &kp, State &kc)
{
		// velocity calculation from accelerometer
		__m128 preVec = _mm_set_ps(0.0f, kp.m_acx, kp.m_acy, kp.m_acz);
		__m128 kVec = _mm_set_ps(0.0f, kc.m_acx, kc.m_acy, kc.m_acz);
		// For Debugging
		float deltaT = kc.m_deltaT/1000;
		std::cout << "deltaT: " << deltaT << '\n'; // for debugging
		sseF.a4 = velCalc(preVec, kVec, deltaT);
		kc.m_velocity = sseF.a[1]; //doesn't matter which index

		//check if GPS is viable absolute
		if (kc.getSat() < 1)
		{
			std::cout << "NO SATS\n";
			kc.m_mph = kc.m_velocity;
		}

		// run the kalman filter
		Kalman::calcGain(kp, kc);
		Kalman::calcCurrentEstimate(kp, kc);
		Kalman::calcErrorNewEstimate(kp, kc);
		//kc.m_estimateVel = kp.m_estimateVel + kc.m_estimateVel;
		//Debug statements
		std::cout << "KG: " <<  kc.m_kalGain << '\n';
		std::cout << "EST VEL: " << kc.m_estimateVel << '\n';
		std::cout << "CURR VEL: "<< kc.m_velocity << '\n';
}

void sensorData(Serial &A, State &Kp, State &K)
{
	if (A.IsConnected())
	{
		std::cout << "Connection Established\n";
	}

	Sleep(2000); //skip initial bad reads
	while (A.IsConnected())
	{
		if (bGraphed)
		{
			mu.lock();
			Kp = std::move(K);
			mu.unlock();
		}
		mu.lock();
		if (A.ReadData(K))
		{
			processingData(Kp, K);
			if (!bEngineLoaded)
			{
				Kp = std::move(K);
			}
		}
		mu.unlock();
		
		Sleep(10);
	}
}

void update(double(&d)[10], float val)
{
	int i = 0;
	for (i = 0; i < (sizeof(d) /sizeof(double)) - 1; i++)
	{	
		d[i] = d[i + 1];
	}
	d[i] = val; 
}

void matlabPlot(Engine *ep, mxArray *T, mxArray *D, mxArray *E, mxArray *X, State &K)
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
	
	bEngineLoaded = true;

	T = mxCreateDoubleMatrix(1, 10, mxREAL);
	D = mxCreateDoubleMatrix(1, 10, mxREAL);
	E = mxCreateDoubleMatrix(1, 10, mxREAL);
	X = mxCreateDoubleMatrix(1, 10, mxREAL);

	memcpy((void *)mxGetPr(T), (void *)timescale, sizeof(timescale));
	memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
	memcpy((void *)mxGetPr(E), (void *)earr, sizeof(earr));
	memcpy((void *)mxGetPr(X), (void *)xarr, sizeof(xarr));

	engPutVariable(ep, "T", T); // time axis
	engPutVariable(ep, "D", D); // gps velocity
	engPutVariable(ep, "E", E); // accel velocity
	engPutVariable(ep, "X", X); // filter result

	bRender = true;
	while (bRender) //animating graph
	{
		bGraphed = false;
		mu.lock();
		std::thread anim1(update, std::ref(darr), abs(K.m_mph));
		std::thread anim2(update, std::ref(earr), abs(K.m_velocity));
		std::thread anim3(update, std::ref(xarr), abs(K.m_estimateVel));
		anim1.join();
		anim2.join();
		anim3.join();
		mu.unlock();
		memcpy((void *)mxGetPr(D), (void *)darr, sizeof(darr));
		memcpy((void *)mxGetPr(X), (void *)xarr, sizeof(xarr));
		memcpy((void *)mxGetPr(E), (void *)earr, sizeof(earr));
		engPutVariable(ep, "D", D);
		engPutVariable(ep, "E", E);
		engPutVariable(ep, "X", X);
		engEvalString(ep, "plot(T, D, '-o', T, E, '-o', T, X, '-o');");
		bGraphed = true;
		Sleep(50);
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
	State *Kprev = new State();
	State *K = new State();
	
	//Initial MatLAB object construction
	Engine *ep;
	mxArray *T = NULL; mxArray *D = NULL;
	mxArray *X = NULL; mxArray *E = NULL;

	std::thread keyboardListen(exitState, std::ref(*Arduino), std::ref(*Kprev), std::ref(*K)); // Passed Serial to destruct connection on exit
	std::thread sensorDataGet(sensorData, std::ref(*Arduino), std::ref(*Kprev), std::ref(*K));
	std::thread matlabDisplay(matlabPlot, std::ref(ep), std::ref(T), std::ref(D), std::ref(E), std::ref(X), std::ref(*K));

	keyboardListen.join();
	sensorDataGet.join();
	matlabDisplay.join();

	return 0;
}