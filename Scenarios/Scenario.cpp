#include <stdlib.h>
#include <ctime>

#include "../lib/script.h"
#include "../lib/utils.h"
#include "Scenario.h"

Scenario::Scenario(int _car, int _drivingStyle, float _setSpeed, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY){
	Vector3 pos, rotation;
	float heading, x, y;
	int hour, minute;
	Hash vehicleHash;

	vehicle = NULL;
	player = NULL;
	ped = NULL;
	camera = NULL;

	srand(std::clock());
	GAMEPLAY::SET_RANDOM_SEED(std::clock());

	if (_initialPosX == -1) x = -3400 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3600 + 3400)));
	else x = _initialPosX;

	if (_initialPosY == -1) y = -3600 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7000 + 3600)));
	else y = _initialPosY;

	heading = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360));

	while (!PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) WAIT(0);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE(x, y, 0, &pos, 1, 300, 300);
	PATHFIND::LOAD_ALL_PATH_NODES(FALSE);

	switch (_car) {
		default:
		case 0: vehicleHash = GAMEPLAY::GET_HASH_KEY("blista"); break; 
		case 1: vehicleHash = GAMEPLAY::GET_HASH_KEY("voltic"); break; //etc
	}
	
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
	if (_initialHour == -1) hour = rand() % 24;
	else hour = _initialHour % 24;
	
	if (_initialMinute == -1) minute = rand() % 60;
	else minute = _initialMinute % 60;

	TIME::SET_CLOCK_TIME(hour, minute, 0);

	switch (_initialWeather) {
		default:
		case 0: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLEAR"); break;
		case 1: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("EXTRASUNNY"); break;
		case 2: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLOUDS"); break;
		case 3: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("OVERCAST"); break;
		case 4: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("RAIN"); break;
		case 5: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLEARING"); break;
		case 6: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("THUNDER"); break;
		case 7: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("SMOG"); break;
		case 8: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("FOGGY"); break;
		case 9: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("XMAS"); break;
		case 10: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("SNOWLIGHT"); break;
		case 11: GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("BLIZZARD"); break;
	}

	lastWeatherChange = std::clock();
	weatherChangeDelay = _weatherChangeDelay;

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

	currentSpeed = 0.0;
	prevSpeed = 0.0;

	//Starts driving!
	switch (_drivingStyle) {
		default:
		case 0: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 786603); break; //Normal (typically used by the game)
		case 1: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 2883621); break; //IgnoreLights
		case 2: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 5); break; //Sometimes Overtake traffic
		case 3: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 1074528293); break; //Rushed
		case 4: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 786468); break; //Avoid traffic
		case 5: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 6); break; //Avoid traffic extreme
		case 6: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 52); break; //Avoid everything
		case 7: AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, _setSpeed, 35); break; //Careful driving	
	}

}
	

void Scenario::step() {
	Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);

	float delay = ((float)(std::clock() - lastWeatherChange)) / CLOCKS_PER_SEC;
	if (delay > weatherChangeDelay) {
		lastWeatherChange = std::clock();
		GAMEPLAY::SET_RANDOM_WEATHER_TYPE(); //TODO: Randomize integer and set default weather from enum list
	}
}

void Scenario::performActions(float throttle, float brake, float steering) {
	CONTROLS::_SET_CONTROL_NORMAL(27, 71, throttle); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, brake); //[0,1];
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, steering); //[-1,1]
}

//m/s
float Scenario::getVehicleSpeed() {
	prevSpeed = currentSpeed;
	prevTime = currentTime;
	currentSpeed = ENTITY::GET_ENTITY_SPEED(vehicle);

	currentTime = std::clock();

	return currentSpeed;
}

//m/s2. Never cal before getVehicleSpeed()
float Scenario::getVehicleAcceleration() {
	return (currentSpeed - prevSpeed) / (((float) (currentTime - prevTime)) / CLOCKS_PER_SEC);
}

//deg. Can sometimes be inacurate :(
float Scenario::getVehicleYawAngle() {
	Vector3 nodepos;
	float nodeHeading;
	float yaw;

	Vector3 pos = ENTITY::GET_ENTITY_COORDS(vehicle, TRUE);
	float vehHeading = 360.0 - ENTITY::GET_ENTITY_HEADING(vehicle);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, &nodepos, &nodeHeading, 1, 300, 300);

	yaw = vehHeading - nodeHeading;
	if (yaw > 90) {
		yaw = 180 - yaw;
	}
	else if (yaw < -90) {
		yaw = yaw + 180;
	}
	
	return yaw;
}

//degs
float Scenario::getVehicleYawRate() {
	Vector3 rates = ENTITY::GET_ENTITY_ROTATION_VELOCITY(vehicle);
	currentYawRate = rates.z*180.0 / 3.14159265359;
	return currentYawRate;
}

//-1, 0 or 1. Never call before getVehicleYawRate()
int Scenario::getVehicleDirection() {
	if (currentYawRate > 0) return 1;
	else if (currentYawRate < 0) return -1;
	else return 0;
}

//[0,1]
float Scenario::getVehicleThrottlePosition() {
	return getFloatValue(vehicle, 0x8D4);
}

//[0,1]
float Scenario::getVehicleBrakePosition() {
	return getFloatValue(vehicle, 0x8D8);
}

//[-1,1]
float Scenario::getVehicleSteeringAngle() {
	return getFloatValue(vehicle, 0x8CC)/-0.628319; //TODO: Depends on vehicle?
}

