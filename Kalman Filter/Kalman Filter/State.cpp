#include "State.h"
State::State()
{
	float m_lat = 0;
	float m_long = 0;
	float m_feet = 0;
	float m_deg = 0;
	float m_mph = 0;
	float m_sat = 0;
	float m_acx = 0;
	float m_acy = 0;
	float m_acz = 0;
	float m_gyx = 0;
	float m_gyy = 0;
	float m_gyz = 0;
	float m_kalGain = .5;
}

void State::dataSet(float sensorData[12])
{
	this->m_lat = sensorData[0];
	this->m_long = sensorData[1];
	this->m_feet = sensorData[2];
	this->m_deg = sensorData[3];
	this->m_mph = sensorData[4];
	this->m_sat = sensorData[5];
	this->m_acx = sensorData[6];
	this->m_acy = sensorData[7];
	this->m_acz = sensorData[8];
	this->m_gyx = sensorData[9];
	this->m_gyy = sensorData[10];
	this->m_gyz = sensorData[11];
	this->m_kalGain = .5;
}

State::State(State &other)
{
	this->m_lat = other.m_lat;
	this->m_long = other.m_long;
	this->m_deg = other.m_deg;
	this->m_mph = other.m_mph;
	this->m_sat = other.m_sat;
	this->m_acx = other.m_acx;
	this->m_acy = other.m_acy;
	this->m_acz = other.m_acz;
	this->m_gyx = other.m_gyx;
	this->m_gyy = other.m_gyy;
	this->m_gyz = other.m_gyz;
	this->m_kalGain = other.m_kalGain;
}

void State::printDataSet()
{
	std::cout << "GPS:\n";
	std::cout << "LAT: " << this->m_lat << " | LONG: " << this->m_long << " | DEG: " << this->m_deg << " | MPH: " << this->m_mph << " | SAT: " << this->m_sat << '\n';
	std::cout << "MPU:\n";
	std::cout << "ACX: " << this->m_acx << " | ACY: " << this->m_acy << " | ACZ: " << this->m_acx << " | GYX: " << this->m_gyx << " | GYY: " << this->m_gyy << " | GYZ: " << this->m_gyz << '\n';
}