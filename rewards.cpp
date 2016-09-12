#include "script.h"
#include "rewards.h"
#include "nodes.h"
#include <math.h>
#include <algorithm>
#include <vector>

//Works
float rewardCollision(Vehicle vehicle, float currentSpeed, float max_speed) {
	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle)) {
		return -1.0;
	}
	return (currentSpeed / max_speed); //It is better to return reward proportional to speed or the vehicle could remain static
}

//Works but is also triggered when colliding with vehicles
float rewardCollisionWithPed(Vehicle vehicle, float currentSpeed, float max_speed) {
	const int numElements = 10;
	const int arrSize = numElements * 2 + 2;
	int nearbyPed[arrSize];
	int count;
	int offsettedID;
	
	nearbyPed[0] = numElements;
	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle)) {
		count = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), nearbyPed, -1);
		if (nearbyPed != NULL) {
			for (int i = 0; i < count; i++) {
				offsettedID = i * 2 + 2;
				if (ENTITY::IS_ENTITY_TOUCHING_ENTITY(vehicle, nearbyPed[offsettedID])) return -1.0;
			}
		}
	}
	return (currentSpeed / max_speed);
}


//Works
float rewardCollisionWithVehicle(Vehicle vehicle, float currentSpeed, float max_speed) {
	const int arrSize = 1024;
	Vehicle vehs[arrSize];
	int count;

	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle)) {
		count = worldGetAllVehicles(vehs, arrSize);
		for (int i = 0; i < count; i++) {
			if (ENTITY::IS_ENTITY_TOUCHING_ENTITY(vehicle, vehs[i])) return -1.0;
		}
	}
	return (currentSpeed / max_speed);
}


//Works
float rewardCollisionWithObject(Vehicle vehicle, float currentSpeed, float max_speed) {
	const int arrSize = 1024;
	Vehicle objs[arrSize];
	int count;

	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle)) {
		count = worldGetAllObjects(objs, arrSize);
		for (int i = 0; i < count; i++) {
			if (ENTITY::IS_ENTITY_TOUCHING_ENTITY(vehicle, objs[i])) return -1.0;
		}
	}
	return (currentSpeed / max_speed);
}

//WORKS
float rewardKeepSpeed(float currentSpeed, float desiredSpeed) {
	float reward = currentSpeed / desiredSpeed;
	if (reward > 1) reward = (1 - reward);
	if (reward < -1) reward = -1;
	return reward;
}

float rewardKeepSecurityDistance(Vector3 currentPosition, float min_distance){
	const int ARR_SIZE = 1024;
	int vehicles[ARR_SIZE];
	std::vector<float> distances;
	Vector3 vehicleCoords;
	float distance;
	float shortest_distance = min_distance;

	int count = worldGetAllVehicles(vehicles, ARR_SIZE);
	for (int i = 0; i < count; i++)
	{
		if (ENTITY::IS_ENTITY_ON_SCREEN(vehicles[i])) {
			vehicleCoords = ENTITY::GET_ENTITY_COORDS(vehicles[i], TRUE);
			distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(vehicleCoords.x, vehicleCoords.y, 0, currentPosition.x, currentPosition.y, 0, 0);
			if (distance < shortest_distance) {
				shortest_distance = distance;
			}
		}
	}

	if (shortest_distance > min_distance) {
		return 1.0;
	}

	return shortest_distance / min_distance;
}

float rewardDistanceReduction(Vector3 currentPosition, Vector3 destination, float initTotalDistance) {
	float currentDistance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(currentPosition.x, currentPosition.y, 0, destination.x, destination.y, 0, 0);
	float reward = 1 - (currentDistance / initTotalDistance);
	if (reward < -1) reward = -1;
	return reward;
}

//Partially works
float rewardCenterOfLane(Vector3 currentPosition, Vector3 forwardVector) { //AND correct direction of traffic!

	//Load the nodes if didn't done before
	if (nodes.size() == 0) {
		if (PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) {
			populateNodes("C:/Program Files/Rockstar Games/Grand Theft Auto V/paths.xml", 1);
			PATHFIND::LOAD_ALL_PATH_NODES(FALSE);
		}
		else return 0.0;
	}

	int nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(currentPosition.x, currentPosition.y, currentPosition.z, 1, 1, 300, 300);
	tNode node = nodes[nodeID];
	tLink link = node.links.at(0);
	if (node.linePoints.size() == 0) return 0.0;

	std::vector<float> distancesToLines;
	float a;
	for (int i = 0; i < node.linePoints.size(); i++) {
		a = GAMEPLAY::GET_ANGLE_BETWEEN_2D_VECTORS(currentPosition.x - node.linePoints.at(i).x, currentPosition.y - node.linePoints.at(i).y, node.linePoints.at(i).x - node.coord.x, node.linePoints.at(i).y - node.coord.y);
		distancesToLines.push_back(abs(GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(currentPosition.x, currentPosition.y, 0, node.linePoints.at(i).x, node.linePoints.at(i).y, 0, FALSE)*SYSTEM::COS(a)));
	}

	std::sort(distancesToLines.begin(), distancesToLines.end());
	
	float reward = 1 - (distancesToLines.at(1) - distancesToLines.at(0)) / (distancesToLines.at(0) + distancesToLines.at(1));
	bool atTheRight = (currentPosition.y)*link.direction.x > (currentPosition.x)*link.direction.y;
	float direction = forwardVector.x*link.direction.x + forwardVector.y*link.direction.y;

	if (atTheRight) {
		if (direction < 0) {
			reward = -reward;
		}		
	}
	else {
		if (direction > 0) {
			reward = -reward;
		}
	}
	
	return reward;
}


//TODO REWARD FOR STOPPING AT STOPS AND TRAFFIC LIGHTS