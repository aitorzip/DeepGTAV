#pragma once

#include "Rewarder.h"
#include <unordered_map>

typedef struct {
	bool disabled; //Disabled (0)
	bool water; //Water (0)
	int speed; //Speed (4)
	int special; //Special (0)
	int density; //Density (-1)
	bool highway; //Highway (0)
	bool noGPS; //NoGps (0)
	bool tunnel; //Tunnel (0)
	bool cantGoLeft; //Cannot Go Left (0)
	bool leftTurnsOnly; //Left Turns Only (0)
	bool offRoad; //Off Road (0)
	bool cantGoRight; //Cannot Go Right (0)
	bool noBigVehicles; //No Big Vehicles (0)
	bool indicateKeepLeft; //Indicate Keep Left (0)
	bool indicateKeepRight; //Indicate Keep Right (0) 
	bool slipLane; //Slip Lane (0)
} tNodeAttr;

typedef struct {
	int width; //Width (0)
	int lanesIn; //Lanes In (1) 
	int lanesOut; //Lanes Out (1) 
	bool narrowRoad; //Narrowroad (0)
	bool gpsBothWays; //GpsBothWays (0)
	bool blockIfNoLanes; //Block If No Lanes (0)
	bool shortcut; //Shortcut (0)
	bool dontUseForNavigation; //Dont Use For Navigation (0)
} tLinkAttr;

typedef struct {
	Vector3 coord;
	bool laneIn;
} tLinePoint;

typedef struct{
	Vector3 coord;
	Vector3 direction;
	tLinkAttr attr;
	std::vector<tLinePoint> linePoints;

	std::string _ref1;
	std::string _ref2;
} tLink;

typedef struct{
	int id;
	Vector3 coord;
	tNodeAttr attr;
	std::vector<tLink> links;
} tNode;

class LaneRewarder : public Rewarder {
private:
	std::unordered_map<int, tNode> nodes;

	//helpers
	void populateNodes(const char* pathsfile);
	void setLinePoints(tNode* node, tLink link);
	std::vector<tLinePoint> getCurrentLanePoints(tLink link, Vector3 currentPosition);
public:
	LaneRewarder(const char* pathsfile);
	~LaneRewarder();
	float computeReward(Vehicle vehicle);
};