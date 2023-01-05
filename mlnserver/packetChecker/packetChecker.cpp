#include "pch.h"
#include "packetChecker.h"



extern "C" IPacketChecker * PacketChecker_Create(logCallback lb, const char* paramStr, const unsigned int paramNum)
{
	return new PacketChecker(lb, paramStr, paramNum);
}

unsigned int PacketChecker_GetHash(IPacketChecker* checker, const unsigned int seq)
{
	return checker->GetHash(seq);
}


PacketChecker::PacketChecker(logCallback lb, const char* paramStr, const unsigned int paramNum)
	:m_lcb(lb)
{

}

PacketChecker::~PacketChecker()
{
}


unsigned int PacketChecker::GetHash(const unsigned int seq)
{
	return 1234;
}