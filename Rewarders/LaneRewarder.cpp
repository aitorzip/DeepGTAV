#include <map>
#include "LaneRewarder.h"
#include "../lib/tinyxml2.h"

LaneRewarder::LaneRewarder(const char* pathsfile) {
	while (nodes.size() == 0) {
		if (PATHFIND::_0xF7B79A50B905A30D(-8192.0f, 8192.0f, -8192.0f, 8192.0f)) {
			populateNodes(pathsfile);
		}
		WAIT(0);
	}
}

LaneRewarder::~LaneRewarder(){
	nodes.clear();
}

float LaneRewarder::computeReward(Vehicle vehicle) {
	std::vector<tLinePoint> pointPair;
	Vector3 laneCenter, currentPosition, forwardVector;
	float d, a, direction, reward;
	int nodeID;
	tNode node;
	tLink link;

	currentPosition = ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);
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

	forwardVector = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);
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


//helpers
void LaneRewarder::populateNodes(const char* pathsfile){
	std::unordered_map<std::string, tNode> tmpnodes;
	tLink *tmplinks = (tLink*)malloc(80592 * sizeof(*tmplinks)); //Too large for the stack, need to store in the heap
	int i = 0;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* object;
	tinyxml2::XMLElement* coords;
	tinyxml2::XMLElement* attr;
	tinyxml2::XMLElement* ref1;
	tinyxml2::XMLElement* ref2;

	doc.LoadFile(pathsfile);
	object = doc.FirstChildElement()->FirstChildElement()->FirstChildElement();
	for (object; object; object = object->NextSiblingElement()) {
		if (object->Attribute("class", "vehiclenode")) {
			tNode node = { 0 };
			node.attr.speed = 4;
			node.attr.density = -1;

			coords = object->FirstChildElement()->FirstChildElement()->FirstChildElement();
			node.coord.x = coords->FloatAttribute("x");
			node.coord.y = coords->FloatAttribute("y");
			node.coord.z = coords->FloatAttribute("z");

			attr = object->FirstChildElement()->NextSiblingElement()->FirstChildElement();
			if (attr != NULL) {
				for (attr; attr; attr = attr->NextSiblingElement()) {
					if (attr->Attribute("name", "Disabled")) {
						node.attr.disabled = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Water")) {
						node.attr.water = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Speed")) {
						node.attr.speed = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Special")) {
						node.attr.special = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Density")) {
						node.attr.density = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Highway")) {
						node.attr.highway = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "NoGps")) {
						node.attr.noGPS = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Tunnel")) {
						node.attr.tunnel = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Cannot Go Left")) {
						node.attr.cantGoLeft = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Left Turns Only")) {
						node.attr.leftTurnsOnly = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Off Road")) {
						node.attr.offRoad = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Cannot Go Right")) {
						node.attr.cantGoRight = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "No Big Vehicles")) {
						node.attr.noBigVehicles = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Indicate Keep Left")) {
						node.attr.indicateKeepLeft = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Indicate Keep Right")) {
						node.attr.indicateKeepRight = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Slip Lane")) {
						node.attr.slipLane = attr->BoolAttribute("value");
					}

				}
			}
			node.id = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(node.coord.x, node.coord.y, node.coord.z, 1, 1, 300, 300);
			//PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(node.coord.x, node.coord.y, node.coord.z, &dummy, &(node.heading), nodetype, 300, 300);
			//node.nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID_WITH_HEADING(node.coord.x, node.coord.y, node.coord.z, 1, &dummy, &(node.heading), nodetype, 300, 300);
			tmpnodes[std::string(object->Attribute("guid"))] = node;
		}
		else if (object->Attribute("class", "vehiclelink")) {
			tLink link = { 0 };
			link.attr.lanesIn = 1;
			link.attr.lanesOut = 1;

			coords = object->FirstChildElement()->FirstChildElement()->FirstChildElement();
			link.coord.x = coords->FloatAttribute("x");
			link.coord.y = coords->FloatAttribute("y");
			link.coord.z = coords->FloatAttribute("z");

			attr = object->FirstChildElement()->NextSiblingElement()->FirstChildElement();
			if (attr != NULL) {
				for (attr; attr; attr = attr->NextSiblingElement()) {
					if (attr->Attribute("name", "Width")) {
						link.attr.width = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Lanes In")) {
						link.attr.lanesIn = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Lanes Out")) {
						link.attr.lanesOut = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Narrowroad")) {
						link.attr.narrowRoad = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "GpsBothWays")) {
						link.attr.gpsBothWays = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Block If No Lanes")) {
						link.attr.blockIfNoLanes = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Shortcut")) {
						link.attr.shortcut = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Dont Use For Navigation")) {
						link.attr.dontUseForNavigation = attr->BoolAttribute("value");
					}
				}
			}

			ref1 = object->FirstChildElement()->NextSiblingElement()->NextSiblingElement()->NextSiblingElement()->FirstChildElement();
			ref2 = ref1->NextSiblingElement();

			link._ref1 = std::string(ref1->Attribute("guid"));
			link._ref2 = std::string(ref2->Attribute("guid"));
			tmplinks[i] = link;
			i++;
		}
	}

	//Puts everything into place for fast searching by nodeID
	tNode node1;
	tNode node2;
	tLink link;
	float m;
	for (i = 0; i < 80592; i++){
		link = tmplinks[i];
		node1 = tmpnodes[link._ref1];
		node2 = tmpnodes[link._ref2];
		m = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(node1.coord.x, node1.coord.y, 0, node2.coord.x, node2.coord.y, 0, FALSE);
		link.direction.x = (node2.coord.x - node1.coord.x) / m; //Unitary vector pointing in the direction of the road
		link.direction.y = (node2.coord.y - node1.coord.y) / m;

		try {
			node1 = nodes.at(node1.id);
			node2 = nodes.at(node2.id);
		}
		catch (const std::out_of_range) {
			//Empty on purpose
		}

		setLinePoints(&node1, link);
		setLinePoints(&node2, link);
		nodes[node1.id] = node1;
		nodes[node2.id] = node2;
	}
	free(tmplinks);
}

void LaneRewarder::setLinePoints(tNode* node, tLink link){

	tLinePoint linePoint;
	int linesIn, linesOut, starti;
	float laneWidth, lineOffset;

	linesIn = link.attr.lanesIn + 1;
	linesOut = link.attr.lanesOut + 1;

	if (node->attr.highway) laneWidth = 6.0;
	else if (link.attr.narrowRoad) laneWidth = 4.2;
	else laneWidth = 5.5;


	if (link.attr.width > 0) { //Usually urban scenarios
		lineOffset = laneWidth / 2;
		starti = 0;
	}
	else if (link.attr.lanesOut == 0){ //Usually interurban scenarios TODO

		if ((link.attr.lanesIn % 2) == 0) {
			lineOffset = 0;
			linesIn = (linesIn / 2) + 1;
			starti = -(linesIn - 1);
		}
		else {
			lineOffset = laneWidth / 2;
			linesIn = linesIn / 2;
			starti = -linesIn;
		}

		linesOut = starti; //Skips linesOut loop
	}
	else { //Urban and interurban scenarios
		lineOffset = 0;
		starti = 1;

		linePoint.coord = node->coord;
		linePoint.laneIn = false;
		link.linePoints.push_back(linePoint);
	}

	linePoint.laneIn = true;
	for (int i = starti; i < linesIn; i++) {
		linePoint.coord.x = node->coord.x + link.direction.y*(lineOffset + laneWidth*i);
		linePoint.coord.y = node->coord.y - link.direction.x*(lineOffset + laneWidth*i);
		linePoint.coord.z = node->coord.z;
		link.linePoints.push_back(linePoint);
	}

	linePoint.laneIn = false;
	for (int i = starti; i < linesOut; i++) {
		linePoint.coord.x = node->coord.x - link.direction.y*(lineOffset + laneWidth*i);
		linePoint.coord.y = node->coord.y + link.direction.x*(lineOffset + laneWidth*i);
		linePoint.coord.z = node->coord.z;
		link.linePoints.push_back(linePoint);
	}

	node->links.push_back(link);
}

std::vector<tLinePoint> LaneRewarder::getCurrentLanePoints(tLink link, Vector3 currentPosition) {
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
