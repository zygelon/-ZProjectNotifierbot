#pragma once
#include <fstream>

namespace EParserMask
{
	using type = unsigned int;
	const type editorStart		= 1;
	const type PIEstart			= 2;
}

EParserMask::type parseUELog(std::wifstream& file, const EParserMask::type parserMask);