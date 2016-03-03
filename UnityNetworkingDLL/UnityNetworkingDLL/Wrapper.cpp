#include "Wrapper.h"
#include "NetworkManager.h"
#include <fstream>
#include <sstream>

NetworkManager instance;

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