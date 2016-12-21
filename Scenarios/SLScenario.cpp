#include "SLScenario.h"

SLScenario::SLScenario(int _car, int _drivingStyle, float _setSpeed, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY) : Scenario(_car, _initialWeather, _weatherChangeDelay, _initialHour, _initialMinute, _initialPosX, _initialPosY){
	currentSpeed = 0.0;
	currentTime = std::clock();

	//Starts driving!
	switch (_drivingStyle) {
		default: break;
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

//m/s
float SLScenario::getVehicleSpeed() {
	prevSpeed = currentSpeed;
	prevTime = currentTime;
	currentTime = std::clock();
	currentSpeed = ENTITY::GET_ENTITY_SPEED(vehicle);

	return currentSpeed;
}

//m/s2. Never cal before getVehicleSpeed()
float SLScenario::getVehicleAcceleration() {
	return (currentSpeed - prevSpeed) / (((float) (currentTime - prevTime)) / CLOCKS_PER_SEC);
}

//deg. Can sometimes be inacurate :(
float SLScenario::getVehicleYawAngle() {
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
float SLScenario::getVehicleYawRate() {
	Vector3 rates = ENTITY::GET_ENTITY_ROTATION_VELOCITY(vehicle);
	currentYawRate = rates.z*180.0 / 3.14159265359;
	return currentYawRate;
}

//-1, 0 or 1. Never call before getVehicleYawRate()
int SLScenario::getVehicleDirection() {
	if (currentYawRate > 0) return 1;
	else if (currentYawRate < 0) return -1;
	else return 0;
}

//[0,1]
float SLScenario::getVehicleThrottlePosition() {
	return getFloatValue(vehicle, 0x8FC);
}

//[0,1]
float SLScenario::getVehicleBrakePosition() {
	return getFloatValue(vehicle, 0x900);
}

//[-1,1]
float SLScenario::getVehicleSteeringAngle() {
	return getFloatValue(vehicle, 0x8F4)/-0.7; //TODO: Depends on vehicle?
}

