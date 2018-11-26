#pragma once
class State
{
private:
	float m_lat;
	float m_long;
	int m_sat; // may be used to validate gps readings
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
};