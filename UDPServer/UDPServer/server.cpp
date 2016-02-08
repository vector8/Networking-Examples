#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <WinSock2.h>
#include <vector>

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
	SOCKADDR_IN serverAddr, senderAddr;
	serverAddr.sin_port = htons(8888);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	std::vector<SOCKADDR_IN> clients;

	if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	char buffer[1024];

	// main loop
	while (1)
	{
		memset(buffer, 0, 1024);
		int slen = sizeof(serverAddr);
		int recv_len;
		
		if ((recv_len = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *) &senderAddr, &slen)) == SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code: " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		std::cout << "Received packet from " << inet_ntoa(senderAddr.sin_addr) << ":" << ntohs(senderAddr.sin_port) << std::endl << "Data: " << buffer << std::endl;

		if (buffer[0] == '0')
		{
			// new connection request
			// add new client
			clients.push_back(senderAddr);

			std::stringstream ss;
			ss << "Client " << inet_ntoa(senderAddr.sin_addr) << ":" << ntohs(senderAddr.sin_port) << " has joined the room.";
			std::string msg = ss.str();

			// broadcast to all other clients that a new one has joined
			for (int i = 0; i < clients.size() - 1; i++)
			{
				if (sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr *) &clients[i], sizeof(clients[i])) == SOCKET_ERROR)
				{
					std::cout << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		else if (buffer[0] == '1')
		{
			// message received, broadcast to all other participants
			std::string msg(buffer);
			msg = msg.substr(1, recv_len - 1);
			std::stringstream ss;
			ss << "[" << inet_ntoa(senderAddr.sin_addr) << ":" << ntohs(senderAddr.sin_port) << "]: " << msg;
			msg = ss.str();

			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].sin_addr.s_addr != senderAddr.sin_addr.s_addr || clients[i].sin_port != senderAddr.sin_port)
				{
					if (sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr *) &clients[i], sizeof(clients[i])) == SOCKET_ERROR)
					{
						std::cout << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
						exit(EXIT_FAILURE);
					}
				}
			}
		}
	}

	// Terminate
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}