#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
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

	// Resolving IP address
	struct hostent *host;

	if ((host = gethostbyname("localhost")) == NULL)
	{
		std::cout << "Failed to resolve name\n";
		WSACleanup();
		return 0;
	}

	// Socket settings
	SOCKADDR_IN sockAddr;
	sockAddr.sin_port = htons(8888);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	// Connect
	if (connect(sock, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) != 0)
	{
		std::cout << "Failed to connect to the server\n";
		WSACleanup();
		return 0;
	}
	
	// non-blocking mode
	// 0: blocking
	// 1: non-blocking mode
	u_long iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
	
	char buffer[1000];
	
	// main loop
	for(;;)
	{
		memset(buffer, 0, 1000);
		int inDataLength = recv(sock, buffer, 1000, 0);
		std::cout << buffer;
		
		int error = WSAGetLastError();
		
		if(error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";
			
			shutdown(sock, SD_SEND);
			closesocket(sock);
		}
	}

	/*// receive
	char buf[256];
	memset(buf, 0, 256);
	int dataLength = recv(sock, buf, 256, 0);
	std::cout << buf;

	// Terminate
	shutdown(sock, SD_SEND);
	closesocket(sock);*/
	WSACleanup();
	system("pause");
	return 0;
}