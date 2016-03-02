#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define BUFFLEN 4096

class NetworkManager
{
private:
	int port;
	std::string serverAddress;
	SOCKET sock;
	struct sockaddr_in sockAddr, senderAddr;
	int slen;
	bool running, isServer;
	std::thread recThread;

	void receiveThread();

public:
	std::string received;
	bool newReceived;

	bool hasError;
	std::string errorMsg;

	NetworkManager();
	~NetworkManager();

	void initialize(int port, std::string serverAddress = "");

	void throwError(std::string errorMsg);

	void cleanUp();

	void send(std::string message);
};