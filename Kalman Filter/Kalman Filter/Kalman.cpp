#include "Kalman.h"

void Kalman::calcGain(State &k0, State &k1)
{
	/*
		KG = E(est)/(E(est)+E(mea))
	*/
	k1.m_kalGain = k1.m_errEst / (k1.m_errEst + g_measure_err);
}

void Kalman::calcCurrentEstimate(State &k0, State &k1)
{
	/*
		EST(t) = EST(t-1) + KG[MEA - EST(t-1)]
	*/
	k1.m_estimateVel = k0.m_estimateVel + (k1.m_kalGain * (k1.m_velocity + k0.m_estimateVel));
}

void Kalman::calcErrorNewEstimate(State &k0, State &k1)
{
	/*
		E(EST(t)) = (E(MEA)E(EST(t)))/(E(MEA)+E(EST(t-1))
		= [1-KG](E(EST(t-1)))
	*/
	k1.m_estimateVel = (1 - k1.m_kalGain)*k0.m_errEst;
}