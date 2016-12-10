#include "RLScenario.h"

RLScenario::RLScenario(int _car, int _initialWeather, int _weatherChangeDelay, int _initialHour, int _initialMinute, int _initialPosX, int _initialPosY, Rewarder& _rewarder) : rewarder(_rewarder), Scenario(_car, _initialWeather, _weatherChangeDelay, _initialHour, _initialMinute, _initialPosX, _initialPosY){
}

void RLScenario::performActions(float throttle, float brake, float steering) {
	CONTROLS::_SET_CONTROL_NORMAL(27, 71, throttle); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, brake); //[0,1];
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, steering); //[-1,1]
}

//TODO
float RLScenario::getReward() {
	return rewarder.computeReward(vehicle);
}