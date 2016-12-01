#include "Environment.h"

Environment::Environment(int _imageWidth, int _imageHeight, Scenario& _scenario) : scenario(_scenario){
	imageWidth = _imageWidth;
	imageHeight = _imageHeight;

	//Screen capture buffer
	GRAPHICS::_GET_SCREEN_ACTIVE_RESOLUTION(&windowWidth, &windowHeight);
	hWnd = ::FindWindow(NULL, "Grand Theft Auto V");
	hWindowDC = GetDC(hWnd);
	hCaptureDC = CreateCompatibleDC(hWindowDC);
	hCaptureBitmap = CreateCompatibleBitmap(hWindowDC, imageWidth, imageHeight);
	SelectObject(hCaptureDC, hCaptureBitmap);
	SetStretchBltMode(hCaptureDC, COLORONCOLOR);
}

Environment::~Environment(){
	ReleaseDC(hWnd, hWindowDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}

void Environment::step() {
	scenario.step();
}