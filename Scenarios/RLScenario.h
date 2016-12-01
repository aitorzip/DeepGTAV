#pragma once

#include "../Rewarders/GeneralRewarder.h"
#include "Scenario.h"

class RLScenario: public Scenario {
public:
	RLScenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY);

	void performActions(float throttle, float brake, float steering);
	float getReward();
};