#include "script.h"


float rewardCollision(Vehicle vehicle, float currentSpeed, float max_speed);
float rewardCollisionWithPed(Vehicle vehicle, float currentSpeed, float max_speed);
float rewardCollisionWithVehicle(Vehicle vehicle, float currentSpeed, float max_speed);
float rewardCollisionWithObject(Vehicle vehicle, float currentSpeed, float max_speed);
float rewardKeepSpeed(float currentSpeed, float desiredSpeed);
float rewardDistanceReduction(Vector3 currentPosition, Vector3 destination, float initTotalDistance);