#pragma once
#include <optional>
#include <string>

using std::optional;

namespace tlgrm
{
	std::optional<int> getChatId(const std::string& tlgrmLogin);
}