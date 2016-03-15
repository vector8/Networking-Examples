// 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<winsock2.h>

#define MSG_SIZE 100
#define UPDATE_INTERVAL 100
#define SERVER "127.0.0.1"

#pragma comment(lib, "ws2_32.lib")

int main()
{
	// Initialize winsock
	WSADATA WsaDat;

	if(WSAStartup(MAKEWORD(2,2), &WsaDat) !=0)
	{
		std::cout<<"Winsock error - Initialization\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Create a socket
	SOCKET Socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(Socket == INVALID_SOCKET)
	{
		std::cout<<"Winsock error - Socket creation failed\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}


	// Setup our socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(8888);
	SockAddr.sin_family=AF_INET;
//	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);
	SockAddr.sin_addr.s_addr= inet_addr(SERVER);


	// Attempt to connect to server
	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
		std::cout<<"Failed to establish connection with server\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Display message from server
	char recBuf[MSG_SIZE];
	char sendBuf[MSG_SIZE];
	memset(recBuf,0,MSG_SIZE-1); // fills the buffer with zeros
	memset(sendBuf,0,MSG_SIZE-1); // fills the buffer with zeros

		//NON-BLOCKING mode
	u_long iMode=1;
	ioctlsocket(Socket, FIONBIO, &iMode);

	int c = 0;
	while(true)
	{
		strcpy(sendBuf,"");
//		std::cout<<"\nEnter message to send ->\n";
//		fgets(buffer,sizeof(buffer),stdin);
		std::sprintf(sendBuf, "%d", c++);
		Sleep(UPDATE_INTERVAL);
		
		 if(send(Socket,sendBuf,sizeof(sendBuf),0)==0)
		 {	
			//0==other side terminated conn
			printf("\nSERVER terminated connection\n");
			Sleep(40);
			closesocket(Socket);
			break;
		 }	
		 //Sleep(10);
	   if(recv(Socket,recBuf,sizeof(recBuf),0) > 0)
	   { 
		std::cout<<std::endl<<recBuf;
		strcpy(recBuf,"");
	   }
	}	


	// Shutdown our socket
	shutdown(Socket,SD_SEND);

	// Close our socket
	closesocket(Socket);

	// Cleanup Winsock
	WSACleanup();
	system("PAUSE");
	return 0;
} // end main


