#pragma once

#include <Windows.h>
#include <fstream>
#include <ctime>

#include "../Rewarders/GeneralRewarder.h"
#include "../Scenarios/Scenario.h"

class Environment {
private:
	Scenario scenario;

	float recordingPeriod;
	std::clock_t lastRecordingTime;
	int nsample;

	std::wstring datasetDir;
	std::ofstream indexFile;

	int windowWidth;
	int windowHeight;
	int imageWidth;
	int imageHeight;

	HWND hWnd;
	HDC hWindowDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;
	CLSID pngClsid;

	void saveSample();
	void performActions(float throttle, float brake, float steering);

public:
	Environment(int _imageWidth, int _imageHeight, int captureFreq, std::string _datasetDir);
	~Environment();

	void setScenario(Scenario _scenario);
	Scenario getScenario();
	void step();
	
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);