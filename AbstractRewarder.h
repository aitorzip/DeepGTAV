#pragma once

#include "script.h"

class AbstractRewarder {
public:
	virtual ~AbstractRewarder() {}
	virtual float computeReward(Vehicle vehicle) = 0;
};