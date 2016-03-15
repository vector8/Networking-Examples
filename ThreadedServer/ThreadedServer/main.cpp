// 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define MAX_HOSTS 32
#define UPDATE_INTERVAL 100
#define MSG_SIZE 100

char hosts[MAX_HOSTS][11];

HANDLE hMutex;

char buf[MSG_SIZE] = { "" };

SOCKET host[MAX_HOSTS];

int h = 0; // host counter

DWORD WINAPI send(LPVOID lpParam)
{
	DWORD dwWaitResult;

	char tempBuf[MSG_SIZE] = { "" };

	while (true)
	{
		dwWaitResult = WaitForSingleObject(hMutex, 50L);

		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			_try
			{
				printf("Got ownership of the object\n");
				strcpy(tempBuf, buf);
				strcpy(buf, "");
			}
			_finally
			{
				if (!ReleaseMutex(hMutex))
				{
					printf("ReleaseMutex() failed\n");
					ExitProcess(0);
				}
				else
				{
					printf("Mutex released\n");
				}
			}
			break;
		case WAIT_TIMEOUT:
			printf("Timeout\n");
			return FALSE;
		case WAIT_ABANDONED:
			printf("Mutex is non-signaled\n");
		}

		// Send buf to all clients
		for (int i = 0; i < h; i++)
		{
			send(host[i], tempBuf, sizeof(tempBuf), 0);
		}

		printf("UPDATES: %d CONTENTS: %s\n", h, tempBuf);
		strcpy(tempBuf, "");

		Sleep(UPDATE_INTERVAL);
	}

	return TRUE;
}

DWORD WINAPI receive(LPVOID lpParam)
{
	printf("Thread created\n");

	SOCKET client = (SOCKET)lpParam;

	char recBuf[MSG_SIZE];
	char sendBuf[MSG_SIZE];

	int sError;

	char tempBuf[10];
	DWORD dwWaitResult;

	int count = 0;

	u_long iMode = 1;
	if (ioctlsocket(client, FIONBIO, &iMode) != NO_ERROR)
	{
		printf("IOCTL error\n");
	}

	// receive loop
	while (true)
	{
		strcpy(recBuf, "");
		if (recv(client, recBuf, sizeof(recBuf), 0) <= 0)
		{
			printf("RECV error\n");
			closesocket(client);
			ExitThread(0);
		}

		sError = WSAGetLastError();
		if (sError != WSAEWOULDBLOCK && strcmp(recBuf, "") != 0)
		{
			printf("Received: %s\n", recBuf);

			dwWaitResult = WaitForSingleObject(hMutex, 50L);

			switch (dwWaitResult)
			{
			case WAIT_OBJECT_0:
				_try
				{
					printf("Mutex is signaled.\n");
					strcat(buf, recBuf);
					strcat(buf, " ");
				}
				_finally
				{
					// Release the mutex object
					if (!ReleaseMutex(hMutex))
					{
						printf("ReleaseMutex() failed \n");
						ExitProcess(0);
					}
					else
					{
						printf("Mutex released\n");
					}
				}

				break;

			case WAIT_TIMEOUT:
				printf("Timeout elapsed\n");
				return FALSE;
			case WAIT_ABANDONED:
				printf("The mutex is set to non_signaled\n");
			}
		}
	}
}

int main()
{
	SOCKET client;
	sockaddr_in from;
	int fromLen = sizeof(from);

	HANDLE aThreads[MAX_HOSTS];
	DWORD threadID;

	WSADATA wsaData;
	sockaddr_in server;

	SOCKET sock;

	hMutex = CreateMutex(NULL, FALSE, NULL);

	if (hMutex == NULL)
	{
		printf("CreateMutex failed");
	}

	printf("Our game server");

	// Start winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 0;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	// Create our socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		return 0;
	}

	if (bind(sock, (sockaddr*)&server, sizeof(server)) != 0)
	{
		return 0;
	}

	if (listen(sock, SOMAXCONN) != 0)
	{
		return 0;
	}

	DWORD sendThread;
	CreateThread(NULL, 0, send, (LPVOID)host, 0, &sendThread);

	while (true)
	{
		client = accept(sock, (struct sockaddr*)&from, &fromLen);
		printf("Client connected\n");

		host[h] = client;

		aThreads[h] = CreateThread(NULL, 0, receive, (LPVOID)client, 0, &threadID);

		h++;
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}