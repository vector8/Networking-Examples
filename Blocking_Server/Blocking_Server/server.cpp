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
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Socket creation failed\n";
		WSACleanup();
		return 0;
	}

	// Socket settings
	SOCKADDR_IN serverInf;
	serverInf.sin_family = AF_INET;
	serverInf.sin_addr.s_addr = INADDR_ANY;
	serverInf.sin_port = htons(8888);

	// Bind socket
	if (bind(sock, (SOCKADDR*)(&serverInf), sizeof(serverInf)) == SOCKET_ERROR)
	{
		std::cout << "Failed to bind socket\n";
		WSACleanup();
		return 0;
	}

	// Listen to socket
	listen(sock, 1);
	SOCKET tempSock = SOCKET_ERROR;
	while (tempSock == SOCKET_ERROR)
	{
		std::cout << "Waiting for connections\n";
		tempSock = accept(sock, NULL, NULL);
	}
	
	sock = tempSock;

	std::cout << "Client connected\n";
	/*char* msg = "Hey listen!!!!!\n";
	send(sock, msg, strlen(msg), 0);*/

	while (1)
	{
		std::cout << "Server: ";
		std::string msg;
		std::getline(std::cin, msg);
		send(sock, msg.c_str(), msg.length(), 0);

		char buf[1024];
		memset(buf, 0, 1024);
		int dataLength = recv(sock, buf, 1024, 0);
		std::cout <<"Client: " << buf << std::endl;
	}

	// Terminate
	shutdown(sock, SD_SEND);
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}