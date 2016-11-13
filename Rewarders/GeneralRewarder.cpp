#include "GeneralRewarder.h"

GeneralRewarder::GeneralRewarder(const char* pathsfile, float setSpeed, float _agressivity): laneRewarder(pathsfile), speedRewarder(setSpeed){
	agressivity = _agressivity;
}

float GeneralRewarder::computeReward(Vehicle vehicle) {
	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle))	return -1.0;
	return agressivity*speedRewarder.computeReward(vehicle) + (1.0f - agressivity)*laneRewarder.computeReward(vehicle);
}