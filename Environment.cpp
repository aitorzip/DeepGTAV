#include <Windows.h>
#include "Environment.h"
#include "script.h"

Environment::Environment() : rewarder("C:/Program Files/Rockstar Games/Grand Theft Auto V/paths.xml", 15.0, 0.5) {
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	hDesktopWnd = GetDesktopWindow();
	hDesktopDC = GetDC(hDesktopWnd);
	hCaptureDC = CreateCompatibleDC(hDesktopDC);
	hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC, hCaptureBitmap);

	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = nScreenWidth;
	bmi.bmiHeader.biHeight = nScreenHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	pPixels = new RGBQUAD[nScreenWidth * nScreenHeight];
}

Environment::~Environment(void) {
	delete[] pPixels;

	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}

void Environment::step() {
	captureScreenPixels();
	performActions(0, 0, 0);
	rewarder.computeReward(scenario.getVehicle());
	scenario.updateCamera();

}

void Environment::captureScreenPixels() {	

	GetDIBits(
		hCaptureDC,
		hCaptureBitmap,
		0,
		nScreenHeight,
		pPixels,
		&bmi,
		DIB_RGB_COLORS
	);

}

void Environment::performActions(float throttle, float brake, float steering) {
	CONTROLS::_SET_CONTROL_NORMAL(27, 71, throttle); //[0,1]
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, brake); //[0,1];
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, steering); //[-1,1]
}

Scenario Environment::getScenario(){
	return scenario;
}

GeneralRewarder Environment::getRewarder(){
	return rewarder;
}