#pragma once

#include "Rewarder.h"
#include "LaneRewarder.h"
#include "SpeedRewarder.h"

class GeneralRewarder : public Rewarder {
private:
	LaneRewarder laneRewarder;
	SpeedRewarder speedRewarder;
	float agressivity;
public:
	GeneralRewarder(const char* pathsfile, float setSpeed, float _agressivity);
	float computeReward(Vehicle vehicle);

};