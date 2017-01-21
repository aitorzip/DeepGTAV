#pragma once

#include <stdlib.h>
#include <ctime>

#include "../lib/script.h"
#include "../lib/utils.h"

class Scenario {
protected:
	Vehicle vehicle;
	Player player;
	Ped ped;
	Cam camera;

	int weatherChangeDelay;
	std::clock_t lastWeatherChange;
	std::clock_t lastSafetyCheck;

public:
	Scenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY);
	virtual void step();

	virtual float getVehicleSpeed(){ return 0; };
	virtual float getVehicleAcceleration(){ return 0; };
	virtual float getVehicleYawAngle(){ return 0; };
	virtual float getVehicleYawRate(){ return 0; };
	virtual int getVehicleDirection(){ return 0; };
	virtual float getVehicleThrottlePosition(){ return 0; };
	virtual float getVehicleBrakePosition(){ return 0; };
	virtual float getVehicleSteeringAngle(){ return 0; };

	virtual void performActions(float throttle, float steering){};
	virtual float getReward(){ return 0; };
};