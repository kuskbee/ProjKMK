#pragma once

#include <string>
#include <map>

namespace util
{

class IniParser
{
public:
	static std::map<std::string, std::string> ParseSection(const std::string& Filepath, const std::string& Section);
};

}// namespace util
