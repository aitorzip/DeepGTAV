#include "Scenario.h"

//TODO: Fix bad initial positions.
Scenario::Scenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY){
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

	if (_initialPosX == -1) x = -3400 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));
	else x = _initialPosX;

	if (_initialPosY == -1) y = -3600 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10600));
	else y = _initialPosY;

	heading = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360));

	while (!PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) WAIT(0);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE(x, y, 0, &pos, 0, 300, 300);
	PATHFIND::LOAD_ALL_PATH_NODES(FALSE);

	if(_car == -1) _car = rand() % 2;
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

	//Time and weather
	if (_initialHour == -1) hour = rand() % 24;
	else hour = _initialHour % 24;

	if (_initialMinute == -1) minute = rand() % 60;
	else minute = _initialMinute % 60;

	TIME::SET_CLOCK_TIME(hour, minute, 0);

	if(_initialWeather == -1) _initialWeather = rand() % 12;
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
	lastSafetyCheck = lastWeatherChange;

	//Puts camera on vehicle
	rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::DESTROY_ALL_CAMS(TRUE);
	camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
	CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 0.5, 0.8, TRUE);
	CAM::SET_CAM_FOV(camera, 60);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
	CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
}


void Scenario::step() {
	std::clock_t now = std::clock();

	Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);

	float delay = ((float)(now - lastSafetyCheck)) / CLOCKS_PER_SEC;
	if (delay > 10) {
		lastSafetyCheck = std::clock();
		//Avoid bad things such as getting killed by the police, robbed, dying in car accidents or other horrible stuff
		PLAYER::SET_EVERYONE_IGNORE_PLAYER(player, TRUE);
		PLAYER::SET_POLICE_IGNORE_PLAYER(player, TRUE);
		PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player); // Never wanted

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
		PED::SET_DRIVER_ABILITY(ped, 100.0);
	} else {
		delay = ((float)(now - lastWeatherChange)) / CLOCKS_PER_SEC;
		if (delay > weatherChangeDelay) {
			lastWeatherChange = std::clock();
			switch (rand() % 12) {
				default:
				case 0: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("CLEAR"); break;
				case 1: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("EXTRASUNNY"); break;
				case 2: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("CLOUDS"); break;
				case 3: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("OVERCAST"); break;
				case 4: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("RAIN"); break;
				case 5: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("CLEARING"); break;
				case 6: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("THUNDER"); break;
				case 7: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("SMOG"); break;
				case 8: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("FOGGY"); break;
				case 9: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("XMAS"); break;
				case 10: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("SNOWLIGHT"); break;
				case 11: GAMEPLAY::SET_WEATHER_TYPE_PERSIST("BLIZZARD"); break;
			}
		}
	}

}

