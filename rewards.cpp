#include "script.h"
#include "rewards.h"
#include "nodes.h"
#include <math.h>
#include <vector>
#include <map>

float rewardFunction(Vehicle vehicle, float desiredSpeed, float aggressivity) {
	Vector3 position = ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);
	float speed = ENTITY::GET_ENTITY_SPEED(vehicle);
	Vector3 forwardVector = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);
	
	if (ENTITY::HAS_ENTITY_COLLIDED_WITH_ANYTHING(vehicle))	return -1.0;
	
	return aggressivity*rewardKeepSpeed(speed, desiredSpeed) + (1.0f - aggressivity)*rewardCenterOfLane(position, forwardVector);
}

float rewardKeepSpeed(float currentSpeed, float desiredSpeed) {
	float reward = currentSpeed / desiredSpeed;
	if (reward > 1.0f) reward = (1.0f - reward);
	if (reward < -1.0f) reward = -1.0f;
	return reward;
}

//It also weights if you are in the wrong way or out of the road (negative reward)
float rewardCenterOfLane(Vector3 currentPosition, Vector3 forwardVector) {

	//Load the nodes if didn't done before
	if (nodes.size() == 0) {
		if (PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) {
			populateNodes("C:/Program Files/Rockstar Games/Grand Theft Auto V/paths.xml", 1);
			PATHFIND::LOAD_ALL_PATH_NODES(FALSE);
		}
		else return 0.0;
	}
	
	if(!PATHFIND::IS_POINT_ON_ROAD(currentPosition.x, currentPosition.y, currentPosition.z, 0)) return -1.0;

	int nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(currentPosition.x, currentPosition.y, currentPosition.z, 1, 1, 300, 300);
	tNode node = nodes[nodeID];
	tLink link = node.links.at(0);
	if (node.linePoints.size() == 0) return 0.0;

	std::multimap<float, bool> distancesToLines;
	float a;
	for (int i = 0; i < node.linePoints.size(); i++) {
		a = GAMEPLAY::GET_ANGLE_BETWEEN_2D_VECTORS(currentPosition.x - node.linePoints.at(i).coord.x, currentPosition.y - node.linePoints.at(i).coord.y, node.linePoints.at(i).coord.x - node.coord.x, node.linePoints.at(i).coord.y - node.coord.y);
		distancesToLines.emplace(abs(SYSTEM::VDIST(currentPosition.x, currentPosition.y, 0, node.linePoints.at(i).coord.x, node.linePoints.at(i).coord.y, 0)*SYSTEM::COS(a)), node.linePoints.at(i).laneIn);
	}

	float d1, d2;
	bool laneIn1, laneIn2;
	std::multimap<float, bool>::const_iterator it = distancesToLines.begin();
	d1 = it->first; laneIn1 = it->second; it++;
	d2 = it->first; laneIn2 = it->second;
	
	float reward = d1/d2;
	float direction = forwardVector.x*link.direction.x + forwardVector.y*link.direction.y;

	if (laneIn1 && laneIn2) {
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
