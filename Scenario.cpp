#include "Scenario.h"
#include "lib/utils.h"
#include "lib/rapidjson/writer.h"
#include "Rewarders\GeneralRewarder.h"
#include "Rewarders\LaneRewarder.h"
#include "Rewarders\SpeedRewarder.h"
#include <time.h>

void Scenario::start(const Value& sc, const Value& dc) {
	if (running) return;

	const char* weatherList[] = { "CLEAR", "EXTRASUNNY", "CLOUDS", "OVERCAST", "RAIN", "CLEARING", "THUNDER", "SMOG", "FOGGY", "XMAS", "SNOWLIGHT", "BLIZZARD", "NEUTRAL", "SNOW" };
	const char* vehicleList[] = { "blista", "voltic", "packer" };
	float x, y, heading;
	int hour, minute, width, height;
	const char* _weather;
	const char* _vehicle;

	Vector3 pos, rotation;
	Hash vehicleHash;

	//Parse options
	srand(std::time(NULL));
	GAMEPLAY::SET_RANDOM_SEED(std::time(NULL));

	if (dc["rate"].IsNull()) rate = 10;
	else rate = dc["rate"].GetInt();
	if (dc["frame"].IsNull()) {
		width = 320; height = 160;
	}
	else {
		if (dc["frame"][0].IsNull()) width = 320;
		else width = dc["frame"][0].GetInt();
		if (dc["frame"][1].IsNull()) height = 160;
		else height = dc["frame"][1].GetInt();
	}
	if (dc["vehicles"].IsNull()) vehicles = false;
	else vehicles = dc["vehicles"].GetBool();
	if (dc["peds"].IsNull()) peds = false;
	else peds = dc["peds"].GetBool();
	if (dc["trafficSigns"].IsNull()) trafficSigns = false;
	else trafficSigns = dc["trafficSigns"].GetBool();
	if (dc["direction"].IsNull()) direction = false;
	else {
		direction = true;
		if (dc["direction"][0].IsNull()) direction = false;
		else dir.x = dc["direction"][0].GetFloat();
		if (dc["direction"][1].IsNull()) direction = false;
		else dir.y = dc["direction"][1].GetFloat();
		if (dc["direction"][2].IsNull()) direction = false;
		else dir.z = dc["direction"][2].GetFloat();
	}
	if (dc["reward"].IsNull()) reward = false;
	else {
		reward = false;
		if (dc["reward"].IsArray()) {
			if (dc["reward"][0].IsFloat() && dc["reward"][1].IsFloat()) {
				delete(rewarder);
				rewarder = new GeneralRewarder((char*)(GetCurrentModulePath() + "paths.xml").c_str(), dc["reward"][0].GetFloat(), dc["reward"][1].GetFloat());
				reward = true;
			}
		}
	}
	if (dc["throttle"].IsNull()) throttle = false;
	else throttle = dc["throttle"].GetBool();
	if (dc["brake"].IsNull()) brake = false;
	else brake = dc["brake"].GetBool();
	if (dc["steering"].IsNull()) steering = false;
	else steering = dc["steering"].GetBool();
	if (dc["speed"].IsNull()) speed = false;
	else speed = dc["speed"].GetBool();
	if (dc["yawRate"].IsNull()) yawRate = false;
	else yawRate = dc["yawRate"].GetBool();
	if (dc["drivingMode"].IsNull()) drivingMode = false;
	else drivingMode = dc["drivingMode"].GetBool();
	if (dc["location"].IsNull()) location = false;
	else location = dc["location"].GetBool();
	if (dc["time"].IsNull()) time = false;
	else time = dc["time"].GetBool();

	const Value& location = sc["location"];
	const Value& time = sc["time"];
	const Value& weather = sc["weather"];
	const Value& vehicle = sc["vehicle"];
	const Value& drivingMode = sc["drivingMode"];

	if (location.IsNull()) {
		x = -3400 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));
		y = -3600 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10600));
	}
	else {
		if (location[0].IsNull()) x = -3400 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));
		else x = location[0].GetFloat();
		if (location[1].IsNull()) y = -3600 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10600));
		else y = location[1].GetFloat();
	}

	if (time.IsNull()) {
		hour = rand() % 24;
		minute = rand() % 60;
	}
	else {
		if (time[0].IsNull()) hour = rand() % 24;
		else hour = time[0].GetInt();
		if (time[1].IsNull()) minute = rand() % 60;
		else minute = time[1].GetInt();
	}

	if (weather.IsNull()) _weather = weatherList[rand() % 14];
	else _weather = weather.GetString();

	if (vehicle.IsNull()) _vehicle = vehicleList[rand() % 3];
	else _vehicle = vehicle.GetString();

	if (drivingMode.IsNull()) {
		_drivingMode = rand() % 4294967296;
		_setSpeed = 1.0*(rand() % 20);
	}
	else {
		if (drivingMode.IsArray()) {
			if (drivingMode[0].IsNull()) _drivingMode = rand() % 4294967296;
			else _drivingMode = drivingMode[0].GetInt();
			if (drivingMode[1].IsNull()) _setSpeed = 1.0*(rand() % 20);
			else _setSpeed = drivingMode[1].GetFloat();
		}
		else _drivingMode = -1;
	}

	//Build scenario
	while (!PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) WAIT(0);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(x, y, 0, &pos, &heading, 0, 300, 300);
	PATHFIND::LOAD_ALL_PATH_NODES(FALSE);

	vehicleHash = GAMEPLAY::GET_HASH_KEY((char*)_vehicle);
	STREAMING::REQUEST_MODEL(vehicleHash);
	while (!STREAMING::HAS_MODEL_LOADED(vehicleHash)) WAIT(0);
	while (!ENTITY::DOES_ENTITY_EXIST(this->vehicle)) {
		this->vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, pos.x, pos.y, pos.z, heading, FALSE, FALSE);
		WAIT(0);
	}
	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(this->vehicle);

	while (!ENTITY::DOES_ENTITY_EXIST(ped)) {
		ped = PLAYER::PLAYER_PED_ID();
		WAIT(0);
	}

	player = PLAYER::PLAYER_ID();
	PLAYER::START_PLAYER_TELEPORT(player, pos.x, pos.y, pos.z, heading, 0, 0, 0);
	while (PLAYER::IS_PLAYER_TELEPORT_ACTIVE()) WAIT(0);

	PED::SET_PED_INTO_VEHICLE(ped, this->vehicle, -1);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);

	TIME::SET_CLOCK_TIME(hour, minute, 0);

	GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char*)_weather);

	rotation = ENTITY::GET_ENTITY_ROTATION(this->vehicle, 1);
	CAM::DESTROY_ALL_CAMS(TRUE);
	camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
	if (strcmp(_vehicle, "packer") == 0) CAM::ATTACH_CAM_TO_ENTITY(camera, this->vehicle, 0, 2.35, 1.7, TRUE);
	else CAM::ATTACH_CAM_TO_ENTITY(camera, this->vehicle, 0, 0.5, 0.8, TRUE);
	CAM::SET_CAM_FOV(camera, 60);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
	CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);

	AI::CLEAR_PED_TASKS(ped);
	if (_drivingMode >= 0) AI::TASK_VEHICLE_DRIVE_WANDER(ped, this->vehicle, _setSpeed, _drivingMode);

	//Create JSON DOM
	d.SetObject();
	Document::AllocatorType& allocator = d.GetAllocator();
	Value a(kArrayType);

	if (vehicles) d.AddMember("vehicles", a, allocator);
	if (peds) d.AddMember("peds", a, allocator);
	if (trafficSigns) d.AddMember("trafficSigns", a, allocator);
	if (direction) d.AddMember("direction", a, allocator);
	if (reward) d.AddMember("reward", 0.0, allocator);
	if (throttle) d.AddMember("throttle", 0.0, allocator);
	if (brake) d.AddMember("brake", 0.0, allocator);
	if (steering) d.AddMember("steering", 0.0, allocator);
	if (speed) d.AddMember("speed", 0.0, allocator);
	if (yawRate) d.AddMember("yawRate", 0.0, allocator);
	if (this->drivingMode) d.AddMember("drivingMode", 0, allocator);
	if (this->location) d.AddMember("location", a, allocator);
	if (this->time) d.AddMember("time", 0, allocator);

	delete(screenCapturer);
	screenCapturer = new ScreenCapturer(width, height);

	running = true;
	lastSafetyCheck = std::clock();
}

void Scenario::config(const Value& sc, const Value& dc) {
	if (!running) return;

	const char* weatherList[] = { "CLEAR", "EXTRASUNNY", "CLOUDS", "OVERCAST", "RAIN", "CLEARING", "THUNDER", "SMOG", "FOGGY", "XMAS", "SNOWLIGHT", "BLIZZARD", "NEUTRAL", "SNOW" };
	const char* vehicleList[] = { "blista", "voltic", "packer" };
	float x, y, heading;
	int hour, minute, _drivingMode, width, height;
	const char* _weather;
	const char* _vehicle;

	Vector3 pos, rotation;
	Hash vehicleHash;

	//Parse options
	srand(std::time(NULL));
	GAMEPLAY::SET_RANDOM_SEED(std::time(NULL));

	if (!dc["rate"].IsNull()) rate = dc["rate"].GetInt();
	if (!dc["frame"].IsNull()) {
		if (!dc["frame"][0].IsNull()) width = dc["frame"][0].GetInt(); 
		else width = screenCapturer->imageWidth;
		if (!dc["frame"][0].IsNull()) height = dc["frame"][1].GetInt();
		else height = screenCapturer->imageHeight;
		
		delete(screenCapturer);
		screenCapturer = new ScreenCapturer(width, height);
	}
	if (!dc["vehicles"].IsNull()) vehicles = dc["vehicles"].GetBool();
	if (!dc["peds"].IsNull()) peds = dc["peds"].GetBool();
	if (!dc["trafficSigns"].IsNull()) trafficSigns = dc["trafficSigns"].GetBool();
	if (!dc["directions"].IsNull()) {
		if (!dc["direction"][0].IsNull()) dir.x = dc["direction"][0].GetFloat();
		if (!dc["direction"][1].IsNull()) dir.y = dc["direction"][1].GetFloat();
		if (!dc["direction"][2].IsNull()) dir.z = dc["direction"][2].GetFloat();
	}
	if (dc["reward"].IsArray()) {
		reward = false;
		if (dc["reward"][0].IsFloat() && dc["reward"][1].IsFloat()) {
			delete(rewarder);
			rewarder = new GeneralRewarder((char*)(GetCurrentModulePath() + "paths.xml").c_str(), dc["reward"][0].GetFloat(), dc["reward"][1].GetFloat());
			reward = true;
		}
	} 
	if (!dc["throttle"].IsNull()) throttle = dc["throttle"].GetBool();
	if (!dc["brake"].IsNull()) brake = dc["brake"].GetBool();
	if (!dc["steering"].IsNull()) steering = dc["steering"].GetBool();
	if (!dc["speed"].IsNull()) speed = dc["speed"].GetBool();
	if (!dc["yawRate"].IsNull()) yawRate = dc["yawRate"].GetBool();
	if (!dc["drivingMode"].IsNull()) drivingMode = dc["drivingMode"].GetBool();
	if (!dc["location"].IsNull()) location = dc["location"].GetBool();
	if (!dc["time"].IsNull()) time = dc["time"].GetBool();

	const Value& location = sc["location"];
	const Value& time = sc["time"];
	const Value& weather = sc["weather"];
	const Value& vehicle = sc["vehicle"];
	const Value& drivingMode = sc["drivingMode"];

	//Create JSON DOM
	d.SetObject();
	Document::AllocatorType& allocator = d.GetAllocator();
	Value a(kArrayType);

	if (vehicles) d.AddMember("vehicles", a, allocator);
	if (peds) d.AddMember("peds", a, allocator);
	if (trafficSigns) d.AddMember("trafficSigns", a, allocator);
	if (direction) d.AddMember("direction", a, allocator);
	if (reward) d.AddMember("reward", 0.0, allocator);
	if (throttle) d.AddMember("throttle", 0.0, allocator);
	if (brake) d.AddMember("brake", 0.0, allocator);
	if (steering) d.AddMember("steering", 0.0, allocator);
	if (speed) d.AddMember("speed", 0.0, allocator);
	if (yawRate) d.AddMember("yawRate", 0.0, allocator);
	if (this->drivingMode) d.AddMember("drivingMode", 0, allocator);
	if (this->location) d.AddMember("location", a, allocator);
	if (this->time) d.AddMember("time", 0, allocator);

	if (!vehicle.IsNull()) {
		_vehicle = vehicle.GetString();
		vehicleHash = GAMEPLAY::GET_HASH_KEY((char*)_vehicle);

		pos = ENTITY::GET_ENTITY_COORDS(ped, 1);
		heading = ENTITY::GET_ENTITY_HEADING(this->vehicle);
		VEHICLE::DELETE_VEHICLE(&(this->vehicle));
		
		STREAMING::REQUEST_MODEL(vehicleHash);
		while (!STREAMING::HAS_MODEL_LOADED(vehicleHash)) WAIT(0);
		while (!ENTITY::DOES_ENTITY_EXIST(this->vehicle)) {
			this->vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, pos.x, pos.y, pos.z, heading, FALSE, FALSE);
			WAIT(0);
		}
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(this->vehicle);

		PED::SET_PED_INTO_VEHICLE(ped, this->vehicle, -1);

		rotation = ENTITY::GET_ENTITY_ROTATION(this->vehicle, 1);
		CAM::DESTROY_ALL_CAMS(TRUE);
		camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
		if (strcmp(_vehicle, "packer") == 0) CAM::ATTACH_CAM_TO_ENTITY(camera, this->vehicle, 0, 2.35, 1.7, TRUE);
		else CAM::ATTACH_CAM_TO_ENTITY(camera, this->vehicle, 0, 0.5, 0.8, TRUE);
		CAM::SET_CAM_FOV(camera, 60);
		CAM::SET_CAM_ACTIVE(camera, TRUE);
		CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
		CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
		CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
	}

	if (!location.IsNull()) {
		if (!location[0].IsNull()) x = location[0].GetFloat();
		if (!location[1].IsNull()) y = location[1].GetFloat();

		while (!PATHFIND::LOAD_ALL_PATH_NODES(TRUE)) WAIT(0);
		PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(x, y, 0, &pos, &heading, 0, 300, 300);
		PATHFIND::LOAD_ALL_PATH_NODES(FALSE);

		PLAYER::START_PLAYER_TELEPORT(player, pos.x, pos.y, pos.z, heading, 0, 0, 0);
		while (PLAYER::IS_PLAYER_TELEPORT_ACTIVE()) WAIT(0);

		ENTITY::SET_ENTITY_QUATERNION(this->vehicle, pos.x, pos.y, pos.z, heading);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(this->vehicle);

		PED::SET_PED_INTO_VEHICLE(ped, this->vehicle, -1);

		rotation = ENTITY::GET_ENTITY_ROTATION(this->vehicle, 1);
		CAM::DESTROY_ALL_CAMS(TRUE);
		camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
		CAM::ATTACH_CAM_TO_ENTITY(camera, this->vehicle, 0, 0.5, 0.8, TRUE);
		CAM::SET_CAM_FOV(camera, 60);
		CAM::SET_CAM_ACTIVE(camera, TRUE);
		CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
		CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
		CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
	}

	if (!time.IsNull()) {
		if (!time[0].IsNull()) hour = time[0].GetInt();
		else hour = TIME::GET_CLOCK_HOURS();
		if (!time[1].IsNull()) minute = time[1].GetInt();
		else minute = TIME::GET_CLOCK_MINUTES();

		TIME::SET_CLOCK_TIME(hour, minute, 0);
	}

	if (!weather.IsNull()) {
		_weather = weather.GetString();
		GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char*)_weather);
	}

	if (!drivingMode.IsNull()) {
		if (drivingMode.IsArray()) {
			if (!drivingMode[0].IsNull()) _drivingMode = drivingMode[0].GetInt();
			if (!drivingMode[1].IsNull()) _setSpeed = drivingMode[1].GetFloat();
		}
		else _drivingMode = -1;
		
		AI::CLEAR_PED_TASKS(ped);
		if (_drivingMode >= 0) AI::TASK_VEHICLE_DRIVE_WANDER(ped, this->vehicle, _setSpeed, _drivingMode);
	}
	
}

void Scenario::run() {
	if (running) {
		std::clock_t now = std::clock();

		Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
		CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);

		if (_drivingMode < 0) {
			CONTROLS::_SET_CONTROL_NORMAL(27, 71, currentThrottle); //[0,1]
			CONTROLS::_SET_CONTROL_NORMAL(27, 72, currentBrake); //[0,1]
			CONTROLS::_SET_CONTROL_NORMAL(27, 59, currentSteering); //[-1,1]
		}
		
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
		}
	}
	scriptWait(0);
}

void Scenario::stop() {
	if (!running) return;
	running = false;
	CAM::DESTROY_ALL_CAMS(TRUE);
	CAM::RENDER_SCRIPT_CAMS(FALSE, TRUE, 500, FALSE, FALSE);
	AI::CLEAR_PED_TASKS(ped);
	setCommands(0.0, 0.0, 0.0);
}

void Scenario::setCommands(float throttle, float brake, float steering) {
	currentThrottle = throttle;
	currentBrake = brake;
	currentSteering = steering;
}

StringBuffer Scenario::generateMessage() {
	StringBuffer buffer;
	buffer.Clear();
	Writer<StringBuffer> writer(buffer);
	
	screenCapturer->capture();

	if (vehicles) setVehiclesList();
	if (peds) setPedsList();
	if (trafficSigns); //TODO
	if (direction) setDirection();
	if (reward) setReward();
	if (throttle) setThrottle();
	if (brake) setBrake();
	if (steering) setSteering();
	if (speed) setSpeed();
	if (yawRate) setYawRate();
	if (drivingMode); //TODO
	if (location) setLocation();
	if (time) setTime();

	d.Accept(writer);

	return buffer;
}

void Scenario::setVehiclesList() {
	const int ARR_SIZE = 1024;
	Vehicle vehicles[ARR_SIZE];
	Value _vehicles(kArrayType);
	Document::AllocatorType& allocator = d.GetAllocator();

	Vector3 FUR; //Front Upper Right
	Vector3 BLL; //Back Lower Lelft
	Vector3 dim; //Vehicle dimensions
	Vector3 upVector, rightVector, forwardVector, position; //Vehicle position
	Hash model;
	Vector3 min;
	Vector3 max;
	Vector3 speedVector;
	float heading, speed;
	int classid;

	Vector3 currentPos = ENTITY::GET_ENTITY_COORDS(vehicle, false);
	Vector3 currentForwardVector = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);

	int count = worldGetAllVehicles(vehicles, ARR_SIZE);
	for (int i = 0; i < count; i++) {
		if (vehicles[i] == vehicle) continue; //Don't process own car!
		if (ENTITY::IS_ENTITY_ON_SCREEN(vehicles[i])) {
			//Check if it is in screen
			ENTITY::GET_ENTITY_MATRIX(vehicles[i], &rightVector, &forwardVector, &upVector, &position); //Blue or red pill
			if (SYSTEM::VDIST2(currentPos.x, currentPos.y, currentPos.z, position.x, position.y, position.z) < 22500) { //150 m.
				if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(vehicle, vehicles[i], 19)){
					//Check if we see it (not occluded)
					model = ENTITY::GET_ENTITY_MODEL(vehicles[i]);
					GAMEPLAY::GET_MODEL_DIMENSIONS(model, &min, &max);

					speedVector = ENTITY::GET_ENTITY_SPEED_VECTOR(vehicles[i], false);
					speed = ENTITY::GET_ENTITY_SPEED(vehicles[i]);
					if (speed > 0) {
						heading = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(speedVector.x - currentForwardVector.x, speedVector.y - currentForwardVector.y);
					}
					else {
						heading = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(forwardVector.x - currentForwardVector.x, forwardVector.y - currentForwardVector.y);
					}

					if (VEHICLE::IS_THIS_MODEL_A_CAR(model)) classid = 0;
					else if (VEHICLE::IS_THIS_MODEL_A_BIKE(model)) classid = 1;
					else if (VEHICLE::IS_THIS_MODEL_A_BICYCLE(model)) classid = 2;
					else if (VEHICLE::IS_THIS_MODEL_A_QUADBIKE(model)) classid = 3;
					else if (VEHICLE::IS_THIS_MODEL_A_BOAT(model)) classid = 4;
					else if (VEHICLE::IS_THIS_MODEL_A_PLANE(model)) classid = 5;
					else if (VEHICLE::IS_THIS_MODEL_A_HELI(model)) classid = 6;
					else if (VEHICLE::IS_THIS_MODEL_A_TRAIN(model)) classid = 7;
					else if (VEHICLE::_IS_THIS_MODEL_A_SUBMERSIBLE(model)) classid = 8;
					else classid = 9; //unknown (ufo?)

					//Calculate size
					dim.x = 0.5*(max.x - min.x);
					dim.y = 0.5*(max.y - min.y);
					dim.z = 0.5*(max.z - min.z);

					FUR.x = position.x + max.y*rightVector.x + max.x*forwardVector.x + max.z*upVector.x;
					FUR.y = position.y + max.y*rightVector.y + max.x*forwardVector.y + max.z*upVector.y;
					FUR.z = position.z + max.y*rightVector.z + max.x*forwardVector.z + max.z*upVector.z;
					//GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(FUR.x, FUR.y, FUR.z, &(FUR.z), 0);
					//FUR.z += 2 * dim.z;

					BLL.x = position.x + min.y*rightVector.x + min.x*forwardVector.x + min.z*upVector.x;
					BLL.y = position.y + min.y*rightVector.y + min.x*forwardVector.y + min.z*upVector.y;
					BLL.z = position.z + min.y*rightVector.z + min.x*forwardVector.z + min.z*upVector.z;
					//GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(BLL.x, BLL.y, 1000.0, &(BLL.z), 0);

					Value _vehicle(kObjectType);

					Value _vector(kArrayType);
					_vector.PushBack(FUR.x - currentPos.x, allocator).PushBack(FUR.y - currentPos.y, allocator).PushBack(FUR.z - currentPos.z, allocator);
					_vehicle.AddMember("FUR", _vector, allocator);
					_vector.SetArray();
					_vector.PushBack(BLL.x - currentPos.x, allocator).PushBack(BLL.y - currentPos.y, allocator).PushBack(BLL.z - currentPos.z, allocator);
					_vehicle.AddMember("BLL", _vector, allocator).AddMember("speed", speed, allocator).AddMember("heading", heading, allocator).AddMember("classID", classid, allocator);

					_vehicles.PushBack(_vehicle, allocator);

					#ifdef DEBUG
					Vector3 edge1 = BLL;
					Vector3 edge2;
					Vector3 edge3;
					Vector3 edge4;
					Vector3 edge5 = FUR;
					Vector3 edge6;
					Vector3 edge7;
					Vector3 edge8;

					edge2.x = edge1.x + 2 * dim.y*rightVector.x;
					edge2.y = edge1.y + 2 * dim.y*rightVector.y;
					edge2.z = edge1.z + 2 * dim.y*rightVector.z;

					edge3.x = edge2.x + 2 * dim.z*upVector.x;
					edge3.y = edge2.y + 2 * dim.z*upVector.y;
					edge3.z = edge2.z + 2 * dim.z*upVector.z;

					edge4.x = edge1.x + 2 * dim.z*upVector.x;
					edge4.y = edge1.y + 2 * dim.z*upVector.y;
					edge4.z = edge1.z + 2 * dim.z*upVector.z;

					edge6.x = edge5.x - 2 * dim.y*rightVector.x;
					edge6.y = edge5.y - 2 * dim.y*rightVector.y;
					edge6.z = edge5.z - 2 * dim.y*rightVector.z;

					edge7.x = edge6.x - 2 * dim.z*upVector.x;
					edge7.y = edge6.y - 2 * dim.z*upVector.y;
					edge7.z = edge6.z - 2 * dim.z*upVector.z;

					edge8.x = edge5.x - 2 * dim.z*upVector.x;
					edge8.y = edge5.y - 2 * dim.z*upVector.y;
					edge8.z = edge5.z - 2 * dim.z*upVector.z;

					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge2.x, edge2.y, edge2.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge4.x, edge4.y, edge4.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge2.x, edge2.y, edge2.z, edge3.x, edge3.y, edge3.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge3.x, edge3.y, edge3.z, edge4.x, edge4.y, edge4.z, 0, 255, 0, 200);

					GRAPHICS::DRAW_LINE(edge5.x, edge5.y, edge5.z, edge6.x, edge6.y, edge6.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge5.x, edge5.y, edge5.z, edge8.x, edge8.y, edge8.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge6.x, edge6.y, edge6.z, edge7.x, edge7.y, edge7.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge7.x, edge7.y, edge7.z, edge8.x, edge8.y, edge8.z, 0, 255, 0, 200);

					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge7.x, edge7.y, edge7.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge2.x, edge2.y, edge2.z, edge8.x, edge8.y, edge8.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge3.x, edge3.y, edge3.z, edge5.x, edge5.y, edge5.z, 0, 255, 0, 200);
					GRAPHICS::DRAW_LINE(edge4.x, edge4.y, edge4.z, edge6.x, edge6.y, edge6.z, 0, 255, 0, 200);
					#endif

				}
			}
		}
	}
			
	d["vehicles"] = _vehicles;
}

void Scenario::setPedsList(){
	const int ARR_SIZE = 1024;
	Ped peds[ARR_SIZE];
	Value _peds(kArrayType);
	Document::AllocatorType& allocator = d.GetAllocator();

	Vector3 FUR; //Front Upper Right
	Vector3 BLL; //Back Lower Lelft
	Vector3 dim; //Vehicle dimensions
	Vector3 upVector, rightVector, forwardVector, position; //Vehicle position
	Hash model;
	Vector3 min;
	Vector3 max;
	Vector3 speedVector;
	float heading, speed;
	int classid;

	Vector3 currentPos = ENTITY::GET_ENTITY_COORDS(vehicle, false);
	Vector3 currentForwardVector = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);

	int count = worldGetAllPeds(peds, ARR_SIZE);
	for (int i = 0; i < count; i++) {
		if (PED::IS_PED_IN_ANY_VEHICLE(peds[i], TRUE)) continue; //Don't process peds in vehicles!
		if (ENTITY::IS_ENTITY_ON_SCREEN(peds[i])) {
			//Check if it is in screen
			ENTITY::GET_ENTITY_MATRIX(peds[i], &rightVector, &forwardVector, &upVector, &position); //Blue or red pill
			if (SYSTEM::VDIST2(currentPos.x, currentPos.y, currentPos.z, position.x, position.y, position.z) < 22500) { //150 m.
				if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, peds[i], 19)){
					//Check if we see it (not occluded)
					model = ENTITY::GET_ENTITY_MODEL(peds[i]);
					GAMEPLAY::GET_MODEL_DIMENSIONS(model, &min, &max);

					speedVector = ENTITY::GET_ENTITY_SPEED_VECTOR(peds[i], false);
					speed = ENTITY::GET_ENTITY_SPEED(peds[i]);
					if (speed > 0) {
						heading = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(speedVector.x - currentForwardVector.x, speedVector.y - currentForwardVector.y);
					}
					else {
						heading = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(forwardVector.x - currentForwardVector.x, forwardVector.y - currentForwardVector.y);
					}

					if (PED::GET_PED_TYPE(peds[i]) == 28) classid = 11; //animal
					else classid = 10;

					//Calculate size
					dim.x = 0.5*(max.x - min.x);
					dim.y = 0.5*(max.y - min.y);
					dim.z = 0.5*(max.z - min.z);

					FUR.x = position.x + max.y*rightVector.x + max.x*forwardVector.x + max.z*upVector.x;
					FUR.y = position.y + max.y*rightVector.y + max.x*forwardVector.y + max.z*upVector.y;
					FUR.z = position.z + max.y*rightVector.z + max.x*forwardVector.z + max.z*upVector.z;
					//GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(FUR.x, FUR.y, 1000.0, &(FUR.z), 0);
					//FUR.z += 2 * dim.z;

					BLL.x = position.x + min.y*rightVector.x + min.x*forwardVector.x + min.z*upVector.x;
					BLL.y = position.y + min.y*rightVector.y + min.x*forwardVector.y + min.z*upVector.y;
					BLL.z = position.z + min.y*rightVector.z + min.x*forwardVector.z + min.z*upVector.z;
					//GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(BLL.x, BLL.y, 1000.0, &(BLL.z), 0);

					Value _ped(kObjectType);

					Value _vector(kArrayType);
					_vector.PushBack(FUR.x - currentPos.x, allocator).PushBack(FUR.y - currentPos.y, allocator).PushBack(FUR.z - currentPos.z, allocator);
					_ped.AddMember("FUR", _vector, allocator);
					_vector.SetArray();
					_vector.PushBack(BLL.x - currentPos.x, allocator).PushBack(BLL.y - currentPos.y, allocator).PushBack(BLL.z - currentPos.z, allocator);
					_ped.AddMember("BLL", _vector, allocator).AddMember("speed", speed, allocator).AddMember("heading", heading, allocator).AddMember("classID", classid, allocator);

					_peds.PushBack(_ped, allocator);

					#ifdef DEBUG
					Vector3 edge1 = BLL;
					Vector3 edge2;
					Vector3 edge3;
					Vector3 edge4;
					Vector3 edge5 = FUR;
					Vector3 edge6;
					Vector3 edge7;
					Vector3 edge8;

					edge2.x = edge1.x + 2 * dim.y*rightVector.x;
					edge2.y = edge1.y + 2 * dim.y*rightVector.y;
					edge2.z = edge1.z + 2 * dim.y*rightVector.z;

					edge3.x = edge2.x + 2 * dim.z*upVector.x;
					edge3.y = edge2.y + 2 * dim.z*upVector.y;
					edge3.z = edge2.z + 2 * dim.z*upVector.z;

					edge4.x = edge1.x + 2 * dim.z*upVector.x;
					edge4.y = edge1.y + 2 * dim.z*upVector.y;
					edge4.z = edge1.z + 2 * dim.z*upVector.z;

					edge6.x = edge5.x - 2 * dim.y*rightVector.x;
					edge6.y = edge5.y - 2 * dim.y*rightVector.y;
					edge6.z = edge5.z - 2 * dim.y*rightVector.z;

					edge7.x = edge6.x - 2 * dim.z*upVector.x;
					edge7.y = edge6.y - 2 * dim.z*upVector.y;
					edge7.z = edge6.z - 2 * dim.z*upVector.z;

					edge8.x = edge5.x - 2 * dim.z*upVector.x;
					edge8.y = edge5.y - 2 * dim.z*upVector.y;
					edge8.z = edge5.z - 2 * dim.z*upVector.z;

					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge2.x, edge2.y, edge2.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge4.x, edge4.y, edge4.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge2.x, edge2.y, edge2.z, edge3.x, edge3.y, edge3.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge3.x, edge3.y, edge3.z, edge4.x, edge4.y, edge4.z, 255, 0, 0, 200);

					GRAPHICS::DRAW_LINE(edge5.x, edge5.y, edge5.z, edge6.x, edge6.y, edge6.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge5.x, edge5.y, edge5.z, edge8.x, edge8.y, edge8.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge6.x, edge6.y, edge6.z, edge7.x, edge7.y, edge7.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge7.x, edge7.y, edge7.z, edge8.x, edge8.y, edge8.z, 255, 0, 0, 200);

					GRAPHICS::DRAW_LINE(edge1.x, edge1.y, edge1.z, edge7.x, edge7.y, edge7.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge2.x, edge2.y, edge2.z, edge8.x, edge8.y, edge8.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge3.x, edge3.y, edge3.z, edge5.x, edge5.y, edge5.z, 255, 0, 0, 200);
					GRAPHICS::DRAW_LINE(edge4.x, edge4.y, edge4.z, edge6.x, edge6.y, edge6.z, 255, 0, 0, 200);
					#endif

				}
			}
		}
	}		
	d["peds"] = _peds;
}


void Scenario::setThrottle(){
	d["throttle"] = getFloatValue(vehicle, 0x8FC);
}

void Scenario::setBrake(){
	d["brake"] = getFloatValue(vehicle, 0x900);
}

void Scenario::setSteering(){
	d["steering"] = getFloatValue(vehicle, 0x8F4) / -0.7;
}

void Scenario::setSpeed(){
	d["speed"] = ENTITY::GET_ENTITY_SPEED(vehicle);
}

void Scenario::setYawRate(){
	Vector3 rates = ENTITY::GET_ENTITY_ROTATION_VELOCITY(vehicle);
	d["yawRate"] = rates.z*180.0 / 3.14159265359;
}

void Scenario::setLocation(){
	Document::AllocatorType& allocator = d.GetAllocator();
	Vector3 pos = ENTITY::GET_ENTITY_COORDS(vehicle, false);
	Value location(kArrayType);
	location.PushBack(pos.x, allocator).PushBack(pos.y, allocator).PushBack(pos.z, allocator);
	d["location"] = location;
}

void Scenario::setTime(){
	d["time"] = TIME::GET_CLOCK_HOURS();
}

void Scenario::setDirection(){
	int direction;
	float distance;
	Document::AllocatorType& allocator = d.GetAllocator();
	PATHFIND::GENERATE_DIRECTIONS_TO_COORD(dir.x, dir.y, dir.z, TRUE, &direction, &vehicle, &distance);
	Value _direction(kArrayType);
	_direction.PushBack(direction, allocator).PushBack(distance, allocator);
	d["direction"] = _direction;
}

void Scenario::setReward() {
	d["reward"] = rewarder->computeReward(vehicle);
}
