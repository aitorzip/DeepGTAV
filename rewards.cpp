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

std::vector<tLinePoint> getCurrentLanePoints(tLink link, Vector3 currentPosition) {
	std::multimap<float, tLinePoint> nearestLines;
	std::vector<tLinePoint> pointPair;
	tLinePoint linePoint1, linePoint2;
	float d1, d2, pos1, pos2;
	
	for (int i = 0; i < link.linePoints.size(); i++) {
		linePoint1 = link.linePoints.at(i);
		nearestLines.emplace(SYSTEM::VDIST(currentPosition.x, currentPosition.y, 0, linePoint1.coord.x, linePoint1.coord.y, 0), linePoint1);
	}

	std::multimap<float, tLinePoint>::const_iterator it = nearestLines.begin();
	d1 = it->first; linePoint1 = it->second; it++;
	d2 = it->first; linePoint2 = it->second;

	pos1 = (link.direction.x)*(currentPosition.y - linePoint1.coord.y) - (link.direction.y)*(currentPosition.x - linePoint1.coord.x);
	pos2 = (link.direction.x)*(currentPosition.y - linePoint2.coord.y) - (link.direction.y)*(currentPosition.x - linePoint2.coord.x);

	if (pos1*pos2 < 0.0) {
		pointPair.push_back(linePoint1);
		pointPair.push_back(linePoint2);
	}

	return pointPair;

	
}

float rewardCenterOfLane(Vector3 currentPosition, Vector3 forwardVector) {
	std::vector<tLinePoint> pointPair;
	Vector3 laneCenter;
	float d, a, direction, reward;
	int nodeID;
	tNode node;
	tLink link;

	//Load the nodes if didn't done before
	if (nodes.size() == 0) {
		if (PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) {
			populateNodes("C:/Program Files/Rockstar Games/Grand Theft Auto V/paths.xml");
			PATHFIND::LOAD_ALL_PATH_NODES(FALSE);
		}
		else return 0.0;
	}

	if (!PATHFIND::IS_POINT_ON_ROAD(currentPosition.x, currentPosition.y, currentPosition.z, 0)) return -1.0;
	
	for (int i = 1; i <= 6; i++) {
		nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(currentPosition.x, currentPosition.y, currentPosition.z, i, 1, 300, 300);
		node = nodes[nodeID];
		if (node.attr.special > 0) continue;
		for (int j = 0; j < node.links.size(); j++){
			link = node.links.at(j);
			if (link.attr.shortcut || link.attr.width == -1) continue;
			pointPair = getCurrentLanePoints(link, currentPosition);
			if (pointPair.size() == 2) break;
		}
		if (pointPair.size() == 2) break;		
	}

	if (pointPair.size() != 2) return -1.0;

	direction = forwardVector.x*link.direction.x + forwardVector.y*link.direction.y;

	laneCenter.x = (pointPair.at(0).coord.x + pointPair.at(1).coord.x) / 2.0f;
	laneCenter.y = (pointPair.at(0).coord.y + pointPair.at(1).coord.y) / 2.0f;

	a = GAMEPLAY::GET_ANGLE_BETWEEN_2D_VECTORS(currentPosition.x - laneCenter.x, currentPosition.y - laneCenter.y, pointPair.at(0).coord.x - laneCenter.x, pointPair.at(0).coord.y - laneCenter.y);
	d = SYSTEM::VDIST(currentPosition.x, currentPosition.y, 0, laneCenter.x, laneCenter.y, 0);
	reward = 1.0f - (d*abs(SYSTEM::COS(a))) / SYSTEM::VDIST(pointPair.at(0).coord.x, pointPair.at(0).coord.y, 0, laneCenter.x, laneCenter.y, 0);

	if (pointPair.at(0).laneIn || pointPair.at(1).laneIn) {
		if (direction < 0.0) {
			reward = -reward;
		}
	}
	else {
		if (direction > 0.0) {
			reward = -reward;
		}
	}

	return reward;
}
