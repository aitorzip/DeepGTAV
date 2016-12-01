/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "lib/script.h"
#include "Environments/RLEnvironment.h"
#include "Scenarios/RLScenario.h"
#include "Environments/SLEnvironment.h"
#include "Scenarios/SLScenario.h"
#include "lib/IniReader.h"
#include "lib/utils.h"

void main()
{	
	CIniReader iniReader((char*)(GetCurrentModulePath() + "config.ini").c_str());
	int mode = iniReader.ReadInteger("environment", "mode", 0);

	if (mode) {
		//Reinforcement Learning Environment
		RLScenario scenario(iniReader.ReadInteger("scenario", "car", 0), iniReader.ReadInteger("scenario", "initialWeather", 0), iniReader.ReadInteger("scenario", "weatherChangeDelay", 600), iniReader.ReadInteger("scenario", "initialHour", -1),
			iniReader.ReadInteger("scenario", "initialMinute", -1), iniReader.ReadInteger("scenario", "initialPosX", -1), iniReader.ReadInteger("scenario", "initialPosY", -1));
		RLEnvironment _environment(iniReader.ReadInteger("environment", "imageWidth", 227), iniReader.ReadInteger("environment", "imageHeight", 227), "localhost", "8000", scenario);
		Environment& environment(_environment);

		int maxDuration = iniReader.ReadInteger("environment", "maxDuration", 1) * 3600 * CLOCKS_PER_SEC;

		while (clock() < maxDuration)
		{
			environment.step();
			WAIT(0);
		}
		TERMINATE();
	}
	else {
		//Supervised Learning Environment
		SLScenario scenario(iniReader.ReadInteger("scenario", "car", 0), iniReader.ReadInteger("scenario", "drivingStyle", 0), iniReader.ReadFloat("scenario", "setSpeed", 15.0), iniReader.ReadInteger("scenario", "initialWeather", 0), iniReader.ReadInteger("scenario", "weatherChangeDelay", 600),
			iniReader.ReadInteger("scenario", "initialHour", -1), iniReader.ReadInteger("scenario", "initialMinute", -1), iniReader.ReadInteger("scenario", "initialPosX", -1), iniReader.ReadInteger("scenario", "initialPosY", -1));
		SLEnvironment _environment(iniReader.ReadInteger("environment", "imageWidth", 227), iniReader.ReadInteger("environment", "imageHeight", 227), iniReader.ReadInteger("environment", "captureFreq", 10), iniReader.ReadString("environment", "datasetDir", "D:\\"), scenario);
		Environment& environment(_environment);

		int maxDuration = iniReader.ReadInteger("environment", "maxDuration", 1) * 3600 * CLOCKS_PER_SEC;

		while (clock() < maxDuration)
		{
			environment.step();
			WAIT(0);
		}
		TERMINATE();
	}

	
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}
