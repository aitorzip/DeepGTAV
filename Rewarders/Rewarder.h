#pragma once

#include "../lib/script.h"

class Rewarder {
public:
	virtual float computeReward(Vehicle vehicle) { return 0.0; };
};