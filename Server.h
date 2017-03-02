#pragma once

#include "Scenario.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "ws2_32")

class Server {
private:
	WSADATA wsaData;
	u_long iMode = 1; //non-blocking socket
	SOCKET ServerSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	bool sendOutputs = false;
	int bytesRead = 0;
	int recvMessageLen = 0;
	int sendMessageLen = 0;
	bool readyToSend = false;
	bool frameSent = false;

	char json[4096];
	StringBuffer message;
	const char* chmessage;
	int messageSize = 0;
	std::clock_t lastSentMessage = std::clock();

	void resetState();

public:
	bool clientConnected = false;
	Scenario scenario;

	Server(unsigned int port);
	void checkRecvMessage();
	void checkSendMessage();
	void checkClient();
};