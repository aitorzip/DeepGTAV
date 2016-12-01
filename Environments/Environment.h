#pragma once

#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <Windows.h>

#include "../Scenarios/Scenario.h"

class Environment {
protected:
	Scenario& scenario;

	int windowWidth;
	int windowHeight;
	int imageWidth;
	int imageHeight;

	HWND hWnd;
	HDC hWindowDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;

public:
	Environment(int _imageWidth, int _imageHeight, Scenario& _scenario);
	~Environment();
	virtual void step();

};