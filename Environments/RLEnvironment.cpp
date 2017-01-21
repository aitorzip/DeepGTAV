#include "RLEnvironment.h"

RLEnvironment::RLEnvironment(int _imageWidth, int _imageHeight, const char* agentAddress, const char* agentPort, RLScenario _scenario) : Environment(_imageWidth, _imageHeight, _scenario), info({ 0 }) {
	pixels = (UINT8*) malloc(imageWidth*imageHeight * 3);
	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biWidth = _imageWidth;
	info.biHeight = -_imageHeight;
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;

	struct addrinfo *ptr, hints;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(agentAddress, agentPort, &hints, &ptr);
	ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
}

RLEnvironment::~RLEnvironment(){
	closesocket(ClientSocket);
	WSACleanup();
}

void RLEnvironment::step() {
	float actions[2], reward;
	int offset = 0, r;
	int buffSize = imageWidth*imageHeight * 3;
	
	Environment::step();
	updateState();

	while (offset < buffSize) {
		r = send(ClientSocket, (const char*)(pixels + offset), buffSize - offset, 0);
		if (r <= 0) break;
		offset = offset + r;
	}
	recv(ClientSocket, (char*) actions, 2*sizeof(float), 0);
	scenario.performActions(actions[0], actions[1]);
	reward = scenario.getReward();
	send(ClientSocket, (const char*) &reward, sizeof(float), 0);
}

void RLEnvironment::updateState() {
	StretchBlt(hCaptureDC, 0, 0, imageWidth, imageHeight, hWindowDC, 0, 0, windowWidth, windowHeight, SRCCOPY | CAPTUREBLT);
	GetDIBits(hCaptureDC, hCaptureBitmap, 0, imageHeight, pixels, (BITMAPINFO*)&info, DIB_RGB_COLORS);

}