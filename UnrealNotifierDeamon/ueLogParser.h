#pragma once
#include <fstream>

namespace EParserMask
{
	using type = unsigned int;
	const type startEditor		= 1;
	const type PIEstart			= 2;
}

bool isActiveBits(const EParserMask::type bitmask, const EParserMask::type bitsToCheck);

bool isJustChangedBits(const EParserMask::type oldBitmask, const EParserMask::type newBitmask, const EParserMask::type bitsToCheck);

void setActiveBit(EParserMask::type& bitmask, const int bit, const bool value);

EParserMask::type parseUELog(std::wifstream& file, const EParserMask::type parserMask);