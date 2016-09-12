#include "nodes.h"
#include "tinyxml2.h"
#include "script.h"
#include <unordered_map>

std::unordered_map<int, tNode> nodes;

void populateNodes(const char* pathsfile, int nodetype){
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
			tNode node = {0};
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
			node.id = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(node.coord.x, node.coord.y, node.coord.z, 1, nodetype, 300, 300);
			//PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(node.coord.x, node.coord.y, node.coord.z, &dummy, &(node.heading), nodetype, 300, 300);
			//node.nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID_WITH_HEADING(node.coord.x, node.coord.y, node.coord.z, 1, &dummy, &(node.heading), nodetype, 300, 300);
			tmpnodes[std::string(object->Attribute("guid"))] = node;
		}
		else if (object->Attribute("class", "vehiclelink")) {
			tLink link = {0};
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

			link.ref1 = std::string(ref1->Attribute("guid"));
			link.ref2 = std::string(ref2->Attribute("guid"));
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
		node1 = tmpnodes[link.ref1];
		node2 = tmpnodes[link.ref2];
		m = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(node1.coord.x, node1.coord.y, 0, node2.coord.x, node2.coord.y, 0, FALSE);
		link.direction.x = (node1.coord.x - node2.coord.x) / m; //Unitary vector pointing in the direction of the road
		link.direction.y = (node1.coord.y - node2.coord.y) / m;
		node1.links.push_back(link);
		node2.links.push_back(link);
		setLinePoints(&node1);
		setLinePoints(&node2);
		nodes[node1.id] = node1;
		nodes[node2.id] = node2;
	}
	free(tmplinks);
}

void setLinePoints(tNode* node){
	Vector3 linePoint;
	tNode node2;
	float lineOffset;
	int lanesIn, lanesOut, nlines;
	int i = 0;
	float laneWidth = 5.5;
	tLink link = node->links.at(0);

	if (link.attr.shortcut) return;

	lanesIn = link.attr.lanesIn;
	lanesOut = link.attr.lanesOut;
	nlines = SYSTEM::FLOOR(1 + (lanesIn + lanesOut) / 2.0f);

	if (node->attr.highway) laneWidth = 6.0;
	else if (link.attr.narrowRoad) laneWidth = 4.2;

	switch (link.attr.width){
	case -1:
		laneWidth = 3.5;
		break;
	case 5:
		laneWidth = 6.0;
	default:
		break;
	}

	lineOffset = laneWidth / 2;
	if ((nlines % 2) == 0) { //Even lanes
		lineOffset = 0;
		i = 1;
		node->linePoints.push_back(node->coord);
	}

	for (; i < nlines; i++) {
		linePoint.x = node->coord.x + lineOffset*link.direction.y + laneWidth*i*link.direction.y; //To the right
		linePoint.y = node->coord.y - lineOffset*link.direction.x - laneWidth*i*link.direction.x;
		linePoint.z = node->coord.z;
		node->linePoints.push_back(linePoint);

		linePoint.x = node->coord.x - lineOffset*link.direction.y - laneWidth*i*link.direction.y; //To the left
		linePoint.y = node->coord.y + lineOffset*link.direction.x + laneWidth*i*link.direction.x;
		linePoint.z = node->coord.z;
		node->linePoints.push_back(linePoint);
	}
}