/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "lib/script.h"
#include "Environments/Environment.h"
#include "Scenarios/Scenario.h"
#include "lib/IniReader.h"
#include "lib/utils.h"

void main()
{	
	CIniReader iniReader((char*)(GetCurrentModulePath() + "config.ini").c_str());
	
	Scenario scenario(iniReader.ReadInteger("scenario", "car", 0), iniReader.ReadInteger("scenario", "drivingStyle", 0),  iniReader.ReadFloat("scenario", "setSpeed", 15.0), iniReader.ReadInteger("scenario", "initialWeather", 0), iniReader.ReadInteger("scenario", "weatherChangeDelay", 600),
		iniReader.ReadInteger("scenario", "initialHour", -1), iniReader.ReadInteger("scenario", "initialMinute", -1), iniReader.ReadInteger("scenario", "initialPosX", -1), iniReader.ReadInteger("scenario", "initialPosY", -1));
	
	Environment environment(iniReader.ReadInteger("environment", "imageWidth", 227), iniReader.ReadInteger("environment", "imageHeight", 227), iniReader.ReadInteger("environment", "captureFreq", 10), iniReader.ReadString("environment", "datasetDir", "D:\\"));
	environment.setScenario(scenario);

	int maxDuration = iniReader.ReadInteger("environment", "maxDuration", 1) * 3600 * CLOCKS_PER_SEC;

	while (clock() < maxDuration)
	{
		environment.step();
		WAIT(0);
	}

	UI::SET_PAUSE_MENU_ACTIVE(TRUE);
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}
