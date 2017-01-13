#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "ws2_32")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "Environment.h"
#include "../Scenarios/RLScenario.h"

class RLEnvironment: public Environment {
private:
	UINT8* pixels;
	BITMAPINFOHEADER info;

	WSADATA wsaData;
	SOCKET ClientSocket = INVALID_SOCKET;

	void updateState();

public:
	RLEnvironment(int _imageWidth, int _imageHeight, const char* agentAddress, const char* agentPort, RLScenario _scenario);
	~RLEnvironment();

	void step();

};