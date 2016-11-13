#pragma once

#include "AbstractRewarder.h"

class SpeedRewarder : public AbstractRewarder {
private:
	float setSpeed;
public:
	SpeedRewarder(float _setSpeed);
	float computeReward(Vehicle vehicle);
};