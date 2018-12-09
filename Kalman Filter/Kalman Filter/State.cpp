#include "State.h"
State::State()
{
	this->m_lat = 0; this->m_long = 0; this->m_feet = 0;
	this->m_deg = 0; this->m_mph = 0; this->m_sat = 0;
	this->m_acx = 0; this->m_acy = 0; this->m_acz = 0;
	this->m_gyx = 0; this->m_gyy = 0; this->m_gyz = 0;
	this->m_kalGain = .5; this->m_deltaT = 0; this->m_estimateVel = 0;
	this->m_errEst = .4;
}

State::~State()
{
	printf("Resource Cleared\n");
	delete this;
}

void State::dataSet(float sensorData[13])
{
	this->m_lat = sensorData[0]; this->m_long = sensorData[1]; this->m_feet = sensorData[2];
	this->m_deg = sensorData[3]; this->m_mph = sensorData[4]; this->m_sat = sensorData[5];
	this->m_acx = sensorData[6]; this->m_acy = sensorData[7]; this->m_acz = sensorData[8];
	this->m_gyx = sensorData[9]; this->m_gyy = sensorData[10]; this->m_gyz = sensorData[11];
	this->m_deltaT = sensorData[12];
}



State& State::operator=(State &&other) 
{
	std::cout << "Move Assignment\n";
	m_acx = other.m_acx; m_acy = other.m_acy; m_acz = other.m_acz;
	m_deltaT = other.m_deltaT; m_velocity = other.m_velocity;
	m_estimateVel = other.m_estimateVel; m_mph = other.m_mph;
	m_errEst = other.m_errEst;
	return *this;
}

void State::printDataSet()
{
	std::cout << "GPS:\n";
	std::cout << "LAT: " << this->m_lat << " | LONG: " << this->m_long << " | DEG: " << this->m_deg << " | MPH: " << this->m_mph << " | SAT: " << this->m_sat << '\n';
	std::cout << "MPU:\n";
	std::cout << "ACX: " << this->m_acx << " | ACY: " << this->m_acy << " | ACZ: " << this->m_acx << " | GYX: " << this->m_gyx << " | GYY: " << this->m_gyy << " | GYZ: " << this->m_gyz << '\n';
}

float State::getSat()
{
	return this->m_sat;
}