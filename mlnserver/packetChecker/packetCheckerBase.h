#pragma once

#include <string>

#ifdef PACKETCHECKER_EXPORTS
#define PACKETCHECKER_API	extern "C" __declspec(dllexport) 
#else
#define PACKETCHECKER_API	extern "C" __declspec(dllimport)
#endif

typedef void(*logCallback)(const char* msg);

class __declspec(dllexport) IPacketChecker {
public:
	virtual unsigned int GetHash(const unsigned int seq) = 0;
};


PACKETCHECKER_API IPacketChecker* PacketChecker_Create(logCallback, const char *paramStr, const unsigned int paramNum);
PACKETCHECKER_API unsigned int PacketChecker_GetHash(IPacketChecker*, const unsigned int seq);



