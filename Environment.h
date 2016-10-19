#pragma once

#include <Windows.h>
#include "GeneralRewarder.h"
#include "Scenario.h"

class Environment {
private:
	int nScreenWidth, nScreenHeight;
	HWND hDesktopWnd;
	HDC hDesktopDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;
	BITMAPINFO bmi;

	RGBQUAD *pPixels;
	Scenario scenario;
	GeneralRewarder rewarder;

	void captureScreenPixels();
	void performActions(float throttle, float brake, float steering);

public:
	Environment();
	~Environment();

	Scenario getScenario();
	GeneralRewarder getRewarder();

	void step();
	
};