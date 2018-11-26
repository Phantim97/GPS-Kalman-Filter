#include "State.h"

State::State()
{
	float m_lat = 0;
	float m_long = 0;
	int m_sat = 0; 
	float m_acx = 0;
	float m_acy = 0;
	float m_acz = 0;
	float m_gyx = 0;
	float m_gyy = 0;
	float m_gyz = 0;
	float m_kalGain= 0;
}

State::State(State &other)
{
	this->m_lat = other.m_lat;
	this->m_long = other.m_long;
	this->m_sat = other.m_sat;
	this->m_acx = other.m_acx;
	this->m_acy = other.m_acy;
	this->m_acz = other.m_acz;
	this->m_gyx = other.m_gyx;
	this->m_gyy = other.m_gyy;
	this->m_gyz = other.m_gyz;
	this->m_kalGain = other.m_kalGain;
}