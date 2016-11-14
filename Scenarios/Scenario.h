#pragma once

#include "../lib/script.h"

class Scenario {
private:
	Vehicle vehicle;
	Player player;
	Ped ped;
	Cam camera;

	int weatherChangeDelay;
	std::clock_t lastWeatherChange;
	std::clock_t lastSafetyCheck;

	float currentSpeed;
	float prevSpeed;
	float currentYawRate;

	std::clock_t currentTime;
	std::clock_t prevTime;

public:
	Scenario() {};
	Scenario(int _car, int _drivingStyle, float _setSpeed, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY);

	void step();
	void performActions(float throttle, float brake, float steering);		

	float getVehicleSpeed();
	float getVehicleAcceleration();
	float getVehicleYawAngle();
	float getVehicleYawRate();
	int getVehicleDirection();
	float getVehicleThrottlePosition();
	float getVehicleBrakePosition();
	float getVehicleSteeringAngle();
};