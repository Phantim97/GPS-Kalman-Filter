#pragma once
#include <iostream>
class State
{
private:
	float m_lat;
	float m_long;
	float m_feet;
	float m_deg;
	float m_mph;
	float m_sat; // float for consistency
	float m_acx;
	float m_acy;
	float m_acz;
	float m_gyx;
	float m_gyy;
	float m_gyz;
	float m_kalGain;

public:
	State();
	State(State &other);
	void dataSet(float sensorData[12]);
	void printDataSet();
};