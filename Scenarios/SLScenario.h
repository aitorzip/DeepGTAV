#pragma once

#include "Scenario.h"

class SLScenario: public Scenario {
private:
	float currentSpeed;
	float prevSpeed;
	float currentYawRate;

	std::clock_t currentTime;
	std::clock_t prevTime;

public:
	SLScenario(int _car, int _drivingStyle, float _setSpeed, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY);

	float getVehicleSpeed();
	float getVehicleAcceleration();
	float getVehicleYawAngle();
	float getVehicleYawRate();
	int getVehicleDirection();
	float getVehicleThrottlePosition();
	float getVehicleBrakePosition();
	float getVehicleSteeringAngle();
};