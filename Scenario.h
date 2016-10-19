#pragma once

#include "script.h"

class Scenario {
private:
	Vehicle vehicle;
	Player player;
	Ped ped;
	Cam camera;

public:
	Scenario();
	void updateCamera();
	Vehicle getVehicle();
	Cam getCamera();
};