#include "SLEnvironment.h"

SLEnvironment::SLEnvironment(int _imageWidth, int _imageHeight, int captureFreq, std::string _datasetDir, SLScenario _scenario) : Environment(_imageWidth, _imageHeight, _scenario){
	recordingPeriod = 1.0/captureFreq;
	datasetDir.assign(_datasetDir.begin(), _datasetDir.end());
	indexFile.open(datasetDir + L"dataset.txt");

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	GetEncoderClsid(L"image/png", &pngClsid);

	nsample = 1;
	lastRecordingTime = std::clock() + ((float) (30.0 * CLOCKS_PER_SEC));
}

void SLEnvironment::step() {
	Environment::step();

	float delay = ((float)(std::clock() - lastRecordingTime)) / CLOCKS_PER_SEC;
	if (delay >= recordingPeriod) {
		lastRecordingTime = std::clock();
		saveSample();
	}
}

void SLEnvironment::saveSample() {	
	StretchBlt(hCaptureDC, 0, 0, imageWidth, imageHeight, hWindowDC, 0, 0, windowWidth, windowHeight, SRCCOPY | CAPTUREBLT);

	std::string line = std::to_string(nsample) + " " + std::to_string(scenario.getVehicleSpeed()) + " " + std::to_string(scenario.getVehicleAcceleration()) + " " + std::to_string(scenario.getVehicleBrakePosition()) + " "
		+ std::to_string(scenario.getVehicleSteeringAngle()) + " " + std::to_string(scenario.getVehicleThrottlePosition()) + " " + std::to_string(scenario.getVehicleYawRate()) + " " + std::to_string(scenario.getVehicleDirection()) + "\n";

	Gdiplus::Bitmap image(hCaptureBitmap, (HPALETTE)0);
	image.Save((datasetDir + std::to_wstring(nsample) + L".png").c_str(), &pngClsid, NULL);
	indexFile.write(line.c_str(), line.length());
	
	nsample++;
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}