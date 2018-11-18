#include <iostream>
#include <vector>
#include <thread>
#include <Windows.h>
#include <xmmintrin.h>

void exitState()
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			exit(1);
		}
	}

}

void processingData()
{
	// Sample loop to display exit functionality
	int i = 0;
	while (1)
	{
		if (i == INT_MAX - 1)
		{
			exit(0);
		}
		std::cout << i << " " << i << " " << i << " " << i << " " << i << " " << i << " " << i << '\n';
		i++;
	}
}

void sensorData()
{
	// Retrieve Sensor Data from Arduino
}

void matlabPlot()
{
	// Render Matlab Graphs
}

int main()
{
	std::thread keyboardListen(exitState);
	std::thread sensorDataGet(sensorData);
	std::thread processingThread(processingData);
	std::thread matlabDisplay(matlabPlot);

	keyboardListen.join();
	sensorDataGet.join();
	processingThread.join();
	matlabDisplay.join();

	return 0;
}