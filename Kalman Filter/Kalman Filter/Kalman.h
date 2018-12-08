#pragma once
#include "State.h"

const float g_measure_err = .4f;

namespace Kalman
{
	void calcGain(State &k0, State &k1);
	void calcCurrentEstimate(State &k0, State &k1);
	void calcErrorNewEstimate(State &k0, State &k1);
}