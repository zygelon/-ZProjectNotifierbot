#include "ueLogParser.h"
#include <string>
#include "xlog.h"
#include "wx/string.h"

using std::wstring;
namespace
{
	const wstring editorStartStr = L"LogUnrealEdMisc: Total Editor Startup Time, took";

	EParserMask::type parseLine(const wstring& line, const EParserMask::type findParserMask)
	{
		EParserMask::type retVal = 0;
		if (findParserMask & EParserMask::startEditor)
		{
			const int foundValue = line.find(editorStartStr) != wstring::npos;
			retVal = EParserMask::startEditor * foundValue;
		}
		return retVal;
	}
}

//[2021.12.03-21.23.04:831][  0]LogUnrealEdMisc: Total Editor Startup Time, took 9.087
EParserMask::type parseUELog(std::wifstream& file, const EParserMask::type parserMask)
{
	EParserMask::type retVal = 0;
	int debugLineNum = 1;
	for (std::wstring line; std::getline(file, line); )
	{
		retVal |= parseLine(line, parserMask);
		++debugLineNum;
	}
	return retVal;
}

bool isJustChangedBits(const EParserMask::type oldBitmask, const EParserMask::type newBitmask, const EParserMask::type bitsToCheck)
{
	return isActiveBits(newBitmask, bitsToCheck) && !isActiveBits(oldBitmask, bitsToCheck);
}

bool isActiveBits(const EParserMask::type bitmask, const EParserMask::type bitsToCheck)
{
	return bitmask & bitsToCheck;
}

void setActiveBit(EParserMask::type& bitmask, const int bit, const bool value)
{
	if (value)
	{
		bitmask |= bit;
	}
	else
	{
		bitmask &= ~bit;
	}
}