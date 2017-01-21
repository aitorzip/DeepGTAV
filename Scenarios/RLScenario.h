#pragma once

#include "../Rewarders/Rewarder.h"
#include "Scenario.h"

class RLScenario: public Scenario {
private:
	Rewarder& rewarder;
public:
	RLScenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY, Rewarder& rewarder);

	void performActions(float throttle, float steering);
	float getReward();
};