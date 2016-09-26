#pragma once

#include "script.h"
#include "nodes.h"
#include <vector>

float rewardFunction(Vehicle vehicle, float desiredSpeed, float aggressivity);
float rewardKeepSpeed(float currentSpeed, float desiredSpeed);
std::vector<tLinePoint> getCurrentLanePoints(tLink link, Vector3 currentPosition);
float rewardCenterOfLane(Vector3 currentPosition, Vector3 forwardVector);