#include "RLEnvironment.h"

RLEnvironment::RLEnvironment(int _imageWidth, int _imageHeight, const char* agentAddress, const char* agentPort, RLScenario _scenario) : Environment(_imageWidth, _imageHeight, _scenario) {	
	pixels = (UINT8*) malloc(imageWidth*imageHeight * 3);
	struct addrinfo *ptr, hints;
	
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
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
	float actions[3], reward;
	Environment::step();

	updateState();
	send(ClientSocket, (const char*)pixels, imageWidth*imageHeight*3, 0);
	recv(ClientSocket, (char*) actions, 3*sizeof(float), 0);
	scenario.performActions(actions[0],actions[1],actions[2]);
	reward = scenario.getReward();
	send(ClientSocket, (const char*) &reward, sizeof(float), 0);
}

void RLEnvironment::updateState() {
	StretchBlt(hCaptureDC, 0, 0, imageWidth, imageHeight, hWindowDC, 0, 0, windowWidth, windowHeight, SRCCOPY | CAPTUREBLT);
	GetBitmapBits(hCaptureBitmap, imageWidth*imageHeight*3, pixels);
}