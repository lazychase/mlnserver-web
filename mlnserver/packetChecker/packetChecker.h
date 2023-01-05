#pragma once

#include "packetCheckerBase.h"

class PacketChecker
	: public IPacketChecker
{
public:
	PacketChecker(logCallback lb, const char* paramStr, const unsigned int paramNum);
	~PacketChecker();

	unsigned int GetHash(const unsigned int seq) override;


private:
	logCallback m_lcb;
};