#include <stdlib.h>
#include <time.h>

#include "script.h"
#include "Scenario.h"

Scenario::Scenario(){
	Vector3 pos, rotation;
	float heading, x, y;
	int hour, minute;
	Hash vehicleHash;

	vehicle = NULL;
	player = NULL;
	ped = NULL;
	camera = NULL;

	srand(time(NULL));
	GAMEPLAY::SET_RANDOM_SEED(time(NULL));

	x = -3400 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3600 + 3400)));
	y = -3600 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7000 + 3600)));
	heading = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360));

	while (!PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) WAIT(0);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE(x, y, 0, &pos, 1, 300, 300);
	PATHFIND::LOAD_ALL_PATH_NODES(FALSE);

	vehicleHash = GAMEPLAY::GET_HASH_KEY("blista"); //TODO: Tesla Model S
	STREAMING::REQUEST_MODEL(vehicleHash);
	while (!STREAMING::HAS_MODEL_LOADED(vehicleHash)) WAIT(0);
	while (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
		vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, pos.x, pos.y, pos.z, heading, FALSE, FALSE); 
		WAIT(0);
	}
	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);
	
	while (!ENTITY::DOES_ENTITY_EXIST(ped)) {
		ped = PLAYER::PLAYER_PED_ID();
		WAIT(0);
	}

	player = PLAYER::PLAYER_ID();
	PLAYER::START_PLAYER_TELEPORT(player, pos.x, pos.y, pos.z, heading, 0, 0, 0);
	while (PLAYER::IS_PLAYER_TELEPORT_ACTIVE()) WAIT(0);

	PED::SET_PED_INTO_VEHICLE(ped, vehicle, -1);	
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);

	PLAYER::SET_EVERYONE_IGNORE_PLAYER(player, true);
	PLAYER::SET_POLICE_IGNORE_PLAYER(player, true);
	PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player); // Never wanted
	PLAYER::SET_MAX_WANTED_LEVEL(0);

	// Put on seat belt
	PED::SET_PED_CONFIG_FLAG(ped, 32, FALSE);

	// Invincible vehicle
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(vehicle, TRUE);

	VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(vehicle, FALSE);
	ENTITY::SET_ENTITY_INVINCIBLE(vehicle, TRUE);
	ENTITY::SET_ENTITY_PROOFS(vehicle, 1, 1, 1, 1, 1, 1, 1, 1);

	// Player invincible
	PLAYER::SET_PLAYER_INVINCIBLE(player, TRUE);

	// Driving characteristics
	PED::SET_DRIVER_AGGRESSIVENESS(ped, 0.0);
	PED::SET_DRIVER_ABILITY(ped, 1.0);

	//Time and weather
	hour = rand() % 24;
	minute = rand() % 60;
	TIME::SET_CLOCK_TIME(hour, minute, 0);
	GAMEPLAY::SET_RANDOM_WEATHER_TYPE();

	//Puts camera on vehicle
	rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::DESTROY_ALL_CAMS(TRUE);
	camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
	CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 0.5, 0.45, TRUE);
	CAM::SET_CAM_FOV(camera, 50);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
	CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
}

void Scenario::updateCamera() {
	Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
	//if (PLAYER::GET_PLAYER_WANTED_LEVEL(player) > 0) PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
}

Vehicle Scenario::getVehicle() {
	return vehicle;
}

Camera Scenario::getCamera() {
	return camera;
}