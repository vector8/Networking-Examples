#include "Wrapper.h"
#include "NetworkManager.h"
#include <fstream>
#include <sstream>

NetworkManager instance;
//unsigned char* recBuf = new unsigned char[BUFFLEN];
//unsigned char* sendBuf = new unsigned char[BUFFLEN];
//bool writeOnce = false;

void initialize(int port, char* serverAddress)
{
	std::string server(serverAddress);

	instance.initialize(port, server);
}

void sendMsg(char* msg)
{
	std::string smsg(msg);
	instance.send(smsg);
}

bool hasReceived()
{
	bool result = instance.newReceived;
	instance.newReceived = false;
	return result;
}

char* getLastReceived()
{
	//memcpy(recBuf, instance.received, instance.recLen);
	//length = instance.recLen;
	//instance.memoryLock = false;

	return (char*)instance.received.c_str();
}

bool hasError()
{
	bool result = instance.hasError;
	instance.hasError = false;
	return result;
}

char* getError()
{
	return (char*)instance.errorMsg.c_str();
}

void cleanUp()
{
	instance.cleanUp();
}