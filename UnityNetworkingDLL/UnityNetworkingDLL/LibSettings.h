#pragma once

#ifdef UNITYNETWORKINGDLL_EXPORTS
#define LIB_API __declspec(dllexport)
#elif UNITYNETWORKINGDLL_IMPORTS
#define LIB_API __declspec(dllimport)
#else
#define LIB_API
#endif