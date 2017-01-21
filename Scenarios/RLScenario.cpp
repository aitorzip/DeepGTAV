#include "RLScenario.h"

RLScenario::RLScenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY, Rewarder& _rewarder) : rewarder(_rewarder), Scenario(_car, _initialWeather, _weatherChangeDelay, _initialHour, _initialMinute, _initialPosX, _initialPosY){
}

//Signal adaptation. Input for control is [-1,1] for throttle/brake  and [-1,1] for steering angle.
void RLScenario::performActions(float throttle, float steering) {
	float brake = 0.0;

	if (throttle < -0.9) { //Full-stop
		throttle = 0.0;
		VEHICLE::SET_VEHICLE_HANDBRAKE(vehicle, TRUE);
	}
	else {
		VEHICLE::SET_VEHICLE_HANDBRAKE(vehicle, FALSE);
		if (throttle < 0.0) {
			brake = -throttle;
			throttle = 0.0;
		}
	}

	CONTROLS::_SET_CONTROL_NORMAL(27, 71, throttle); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, brake); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, steering); //[-1,1]
}

float RLScenario::getReward() {
	return rewarder.computeReward(vehicle);
}
