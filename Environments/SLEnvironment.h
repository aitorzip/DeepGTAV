#pragma once

#include "Environment.h"
#include "../Scenarios/SLScenario.h"

#include <fstream>
#include <ctime>
#include <gdiplus.h>
#include <string>
#include "../lib/script.h"

#pragma comment(lib,"gdiplus.lib")

class SLEnvironment: public Environment {
private:
	float recordingPeriod;
	std::clock_t lastRecordingTime;
	int nsample;

	CLSID pngClsid;
	std::wstring datasetDir;
	std::ofstream indexFile;

	void saveSample();

public:
	SLEnvironment(int _imageWidth, int _imageHeight, int captureFreq, std::string _datasetDir, SLScenario _scenario);
	void step();
	
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);