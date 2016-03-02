#pragma once

#include "LibSettings.h"

#ifdef __cplusplus
extern "C"
{
#endif

	LIB_API void initialize(int port, char* serverAddress);
	LIB_API void sendMsg(char* msg);
	LIB_API bool hasReceived();
	LIB_API char* getLastReceived();
	LIB_API bool hasError();
	LIB_API char* getError();
	LIB_API void cleanUp();

#ifdef __cplusplus
}
#endif