#include "NetworkManager.h"
#include <string>

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::initialize(int port, std::string serverAddress)
{
	this->port = port;
	this->serverAddress = serverAddress;
	isServer = (serverAddress.length() == 0);

	WSADATA wsa;

	//initialise Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		throwError("Failed. Error Code: " + WSAGetLastError());
	}

	//create socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		throwError("socket() failed with error code: " + WSAGetLastError());
	}

	//setup address structure
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if (isServer)
	{
		sockAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else	// we are a client - connect to server at serverAddress
	{
		sockAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	}

	slen = sizeof(sockAddr);

	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);

	if (isServer)
	{
		if (bind(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		{
			throwError("Bind failed with error code: " + WSAGetLastError());
		}
	}
	else
	{
		// send connect msg
		std::string connectMsg = "0";
		sendto(sock, connectMsg.c_str(), connectMsg.length(), 0, (struct sockaddr *) &sockAddr, slen);
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::string errorMsg = "Error code: " + std::to_string(error);
			throwError(errorMsg);
		}
	}

	running = true;
	recThread = std::thread(&NetworkManager::receiveThread, this);
}

void NetworkManager::throwError(std::string msg)
{
	hasError = true;
	errorMsg = msg;
}

void NetworkManager::receiveThread()
{
	bool connected = false;
	char buffer[BUFFLEN];
	while (running)
	{
		memset(buffer, '\0', BUFFLEN);
		int recLenTemp;
		if (isServer)
		{
			recLenTemp = recvfrom(sock, buffer, BUFFLEN, 0, (struct sockaddr *) &senderAddr, &slen);
		}
		else
		{
			recLenTemp = recvfrom(sock, buffer, BUFFLEN, 0, (struct sockaddr *) &sockAddr, &slen);
		}

		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::string errorMsg = "Error code: " + std::to_string(error);
			throwError(errorMsg);
		}
		else if (error == 0)
		{
			if (isServer && !connected)
			{
				connected = true;
				continue;
			}

			received = std::string(buffer);
			newReceived = true;
		}
	}
}

void NetworkManager::cleanUp()
{
	running = false;
	recThread.join();
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	WSACleanup();
}

void NetworkManager::send(std::string message)
{
	if (isServer)
	{
		if (sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr*) &senderAddr, slen) == SOCKET_ERROR)
		{
			throwError("sendto() failed with error code: " + WSAGetLastError());
		}
	}
	else
	{
		if (sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr*) &sockAddr, slen) == SOCKET_ERROR)
		{
			throwError("sendto() failed with error code: " + WSAGetLastError());
		}
	}
}