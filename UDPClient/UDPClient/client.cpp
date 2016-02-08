#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

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
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Socket creation failed\n";
		WSACleanup();
		return 0;
	}

	// Socket settings
	SOCKADDR_IN sockAddr;
	sockAddr.sin_port = htons(8888);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("10.20.5.169");

	// non-blocking mode
	// 0: blocking
	// 1: non-blocking mode
	u_long iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
	
	char buffer[1024];
	
	// main loop
	while (1)
	{
		memset(buffer, 0, 1024);
		std::string msg;
		std::getline(std::cin, msg);
		int slen = sizeof(sockAddr);
		sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr *) &sockAddr, slen);

		int error = WSAGetLastError();

		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";

			shutdown(sock, SD_SEND);
			closesocket(sock);
		}

		recvfrom(sock, buffer, 1024, 0, (struct sockaddr *) &sockAddr, &slen);

		if(error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";
			
			shutdown(sock, SD_SEND);
			closesocket(sock);
		}
	}

	// Terminate
	WSACleanup();
	system("pause");
	return 0;
}