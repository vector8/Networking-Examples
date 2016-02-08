#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

SOCKET sock;
SOCKADDR_IN sockAddr;
int slen;
bool running;

void receive()
{
	char buffer[1024];
	while (running)
	{
		memset(buffer, 0, 1024);
		recvfrom(sock, buffer, 1024, 0, (struct sockaddr *) &sockAddr, &slen);

		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";

			shutdown(sock, SD_SEND);
			closesocket(sock);
			break;
		}
		else if (error == 0)
		{
			std::cout << buffer << std::endl;
		}
	}
}

int main()
{
	// Initialize
	WSADATA WsaData;

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		std::cout << "Winsock FAILED!\r\n";
		WSACleanup();
		return 0;
	}

	// Create socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Socket creation failed\n";
		WSACleanup();
		return 0;
	}

	// Socket settings
	sockAddr.sin_port = htons(8888);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	slen = sizeof(sockAddr);

	// non-blocking mode
	// 0: blocking
	// 1: non-blocking mode
	u_long iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
	
	char buffer[1024];

	std::string connectMsg = "0";
	sendto(sock, connectMsg.c_str(), connectMsg.length(), 0, (struct sockaddr *) &sockAddr, slen);
	int error = WSAGetLastError();
	if (error != WSAEWOULDBLOCK && error != 0)
	{
		std::cout << "Error code : " << error << "\n";

		running = false;
		shutdown(sock, SD_SEND);
		closesocket(sock);
	}

	running = true;
	std::thread recThread(receive);
	
	// main loop
	while (1)
	{
		memset(buffer, 0, 1024);
		std::string msg;
		std::getline(std::cin, msg);
		msg = "1" + msg;
		sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr *) &sockAddr, slen);

		error = WSAGetLastError();

		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";

			running = false;
			shutdown(sock, SD_SEND);
			closesocket(sock);
			break;
		}
	}

	running = false;
	recThread.join();

	// Terminate
	WSACleanup();
	system("pause");
	return 0;
}