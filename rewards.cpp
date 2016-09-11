#include "script.h"
#include "rewards.h"

float rewardCollision(Vehicle vehicle, float currentSpeed, float max_speed) {
	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle)) {
		return (-currentSpeed / max_speed);
	}
	else return (currentSpeed / max_speed); //It is better to return reward proportional to speed or the vehicle could remain static
}

float rewardCollisionWithPed(Vehicle vehicle, float currentSpeed, float max_speed) {

	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle) && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(vehicle)) {
		ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(vehicle);
		return (-currentSpeed / max_speed);
	}
	else return (currentSpeed / max_speed);
}

float rewardCollisionWithVehicle(Vehicle vehicle, float currentSpeed, float max_speed) {

	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle) && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ANY_VEHICLE(vehicle)) {
		ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(vehicle);
		return (-currentSpeed / max_speed);
	}
	else return (currentSpeed / max_speed);
}

float rewardCollisionWithObject(Vehicle vehicle, float currentSpeed, float max_speed) {

	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle) && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ANY_OBJECT(vehicle)) {
		ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(vehicle);
		return (-currentSpeed / max_speed);
	}
	else return (currentSpeed / max_speed);
}

float rewardKeepSpeed(float currentSpeed, float desiredSpeed) {
	float reward = currentSpeed / desiredSpeed;
	if (reward > 1) reward = (1 - reward);
	if (reward < -1) reward = -1;
	return reward;
}

float rewardDistanceReduction(Vector3 currentPosition, Vector3 destination, float initTotalDistance) {
	float currentDistance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(currentPosition.x, currentPosition.y, 0, destination.x, destination.y, 0, 0);
	float reward = 1 - (currentDistance / initTotalDistance);
	if (reward < -1) reward = -1;
	return reward;
}

//TODO REWARD TO LANE CENTERS (AND CORRECT TRAFFIC DIRECTION)
//TODO REWARD FOR STOPPING AT STOPS AND TRAFFIC LIGHTS