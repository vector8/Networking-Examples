#include "Wrapper.h"
#include "NetworkManager.h"

NetworkManager instance;

void initialize(int port, char* serverAddress)
{
	std::string server(serverAddress);

	instance.initialize(port, server);
}

void sendMsg(char* msg, int msgLength)
{
	instance.send(msg, msgLength);
}

bool hasReceived()
{
	bool result = instance.newReceived;
	instance.newReceived = false;
	return result;
}

char* getLastReceived()
{
	return instance.received;
}

bool hasError()
{
	bool result = instance.hasError;
	instance.hasError = false;
	return result;
}

const char* getError()
{
	return instance.errorMsg.c_str();
}

void cleanUp()
{
	instance.cleanUp();
}