#pragma once

#include "script.h"

float rewardFunction(Vehicle vehicle, float desiredSpeed, float aggressivity);
float rewardKeepSpeed(float currentSpeed, float desiredSpeed);
float rewardCenterOfLane(Vector3 currentPosition, Vector3 forwardVector);