#include "NetworkManager.h"

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

	//printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		throwError("Failed. Error Code: " + WSAGetLastError());
	}
	//printf("Initialised.\n");

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
		char buffer[1024];

		// send connect msg
		std::string connectMsg = "0";
		sendto(sock, connectMsg.c_str(), connectMsg.length(), 0, (struct sockaddr *) &sockAddr, slen);
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::string error = "Error code : " + error + "\n";
			throwError(error);
		}
	}

	running = true;
	recThread = std::thread(&NetworkManager::receiveThread, this);
}

void NetworkManager::throwError(std::string errorMsg)
{
	// TODO: pass the error up to unity
}

void NetworkManager::receiveThread()
{
	bool connected = false;
	char buffer[1024];
	while (running)
	{
		memset(buffer, 0, 1024);
		recvfrom(sock, buffer, 1024, 0, (struct sockaddr *) &sockAddr, &slen);

		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::string error = "Error code : " + error + "\n";
			throwError(error);
		}
		else if (error == 0)
		{
			if (isServer && !connected)
			{
				connected = true;
				continue;
			}

			// TODO: do something with the memory in buffer

			//memcpy(otherTrans, buffer, sizeof(float) * 2);
		}
	}
}

void NetworkManager::cleanUp()
{
	running = false;
	recThread.join();
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

void NetworkManager::send(char* message, int length)
{
	memcpy(msg, message, length);

	if (sendto(sock, msg, length, 0, (struct sockaddr*) &sockAddr, slen) == SOCKET_ERROR)
	{
		throwError("sendto() failed with error code: " + WSAGetLastError());
	}

	memset(msg, '\0', BUFFLEN);
}