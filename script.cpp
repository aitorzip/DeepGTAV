/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "lib/script.h"
#include "Environments/RLEnvironment.h"
#include "Scenarios/RLScenario.h"
#include "Rewarders/GeneralRewarder.h"
#include "Environments/SLEnvironment.h"
#include "Scenarios/SLScenario.h"
#include "lib/IniReader.h"
#include "lib/utils.h"

void ScriptMain()
{
	Environment* environment;

	CIniReader iniReader((char*)(GetCurrentModulePath() + "config.ini").c_str());
	int mode = iniReader.ReadInteger("common", "mode", 0);
	int car = iniReader.ReadInteger("common", "car", 0);
	int imageWidth = iniReader.ReadInteger("common", "imageWidth", 200);
	int imageHeight = iniReader.ReadInteger("common", "imageHeight", 66);
	int initialWeather = iniReader.ReadInteger("common", "initialWeather", -1);
	int weatherChangeDelay = iniReader.ReadInteger("common", "weatherChangeDelay", 600);
	int initialHour = iniReader.ReadInteger("common", "initialHour", -1);
	int initialMinute = iniReader.ReadInteger("common", "initialMinute", -1);
	int initialPosX = iniReader.ReadInteger("common", "initialPosX", -1);
	int initialPosY = iniReader.ReadInteger("common", "initialPosY", -1);
	int maxDuration = iniReader.ReadInteger("common", "maxDuration", 1) * 3600 * CLOCKS_PER_SEC;

	srand(GetTickCount());

	if (mode) {
		//Reinforcement Learning Environment
		int reward = iniReader.ReadInteger("reinforcement", "reward", 0);
		float desiredSpeed = iniReader.ReadFloat("reinforcement", "desiredSpeed", 15.0);
		float desiredAgressivity = iniReader.ReadFloat("reinforcement", "desiredAgressivity", 0.5);
		std::string host = iniReader.ReadString("reinforcement", "host", "localhost");
		std::string port = iniReader.ReadString("reinforcement", "port", "8000");

		Rewarder* rewarder;

		switch (reward) {
		case 0:
			rewarder = new GeneralRewarder((char*)(GetCurrentModulePath() + "paths.xml").c_str(), desiredSpeed, desiredAgressivity);
			break;
		case 1:
			rewarder = new LaneRewarder((char*)(GetCurrentModulePath() + "paths.xml").c_str());
			break;
		case 2:
			rewarder = new SpeedRewarder(15.0);
			break;
		}

		RLScenario scenario(car, initialWeather, weatherChangeDelay, initialHour, initialMinute, initialPosX, initialPosY, *rewarder);
		environment = new RLEnvironment(imageWidth, imageHeight, host.c_str(), port.c_str(), scenario);

	}
	else {
		//Supervised Learning Environment
		int drivingStyle = iniReader.ReadInteger("supervised", "drivingStyle", 0);
		float setSpeed = iniReader.ReadFloat("supervised", "setSpeed", 15.0);
		int captureFreq = iniReader.ReadInteger("supervised", "captureFreq", 10);
		std::string datasetDir = iniReader.ReadString("supervised", "datasetDir", "E:\\GTAVDataset\\");

		SLScenario scenario(car, drivingStyle, setSpeed, initialWeather, weatherChangeDelay, initialHour, initialMinute, initialPosX, initialPosY);
		environment = new SLEnvironment(imageWidth, imageHeight, captureFreq, datasetDir, scenario);
	}


	while (clock() < maxDuration)
	{
		(*environment).step();
		WAIT(0);
	}
	TERMINATE();

}
