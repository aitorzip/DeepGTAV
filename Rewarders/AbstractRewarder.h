#pragma once

#include "../lib/script.h"

class AbstractRewarder {
public:
	virtual float computeReward(Vehicle vehicle) = 0;
};