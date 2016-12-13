#include "RLScenario.h"

RLScenario::RLScenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY, Rewarder& _rewarder) : rewarder(_rewarder), Scenario(_car, _initialWeather, _weatherChangeDelay, _initialHour, _initialMinute, _initialPosX, _initialPosY){
}

//Signal adaptation as input from control is [0,1] for throttle and [0,1] for brake. While in-game capture is [-1,1] for throttle (negative in reverse) and [0,1] for brake.
void RLScenario::performActions(float throttle, float brake, float steering) {
	if (brake > 0.95) { //Full-stop
		brake = 0.0;
		VEHICLE::SET_VEHICLE_HANDBRAKE(vehicle, TRUE);
	}
	else {
		VEHICLE::SET_VEHICLE_HANDBRAKE(vehicle, FALSE);
		if (throttle < 0.0) { //Reverse
			float tmp = brake;
			brake = -throttle;
			throttle = tmp;
		}
	}

	CONTROLS::_SET_CONTROL_NORMAL(27, 71, throttle); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, brake); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, steering); //[-1,1]
}

float RLScenario::getReward() {
	return rewarder.computeReward(vehicle);
}
