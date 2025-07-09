#include "IniParser.h"
#include <fstream>
#include <sstream>

namespace util
{

std::map<std::string, std::string> IniParser::ParseSection(const std::string& Filepath, const std::string& Section)
{
    std::ifstream in(Filepath);

    std::map<std::string, std::string> KeyValueMap;
    std::string Line;
    bool bInSection = false;

    std::string SectionTag = "[" + Section + "]";

    auto trim = [](std::string& s) {
        const char* ws = " \t\r\n";
        s.erase(0, s.find_first_not_of(ws));
        s.erase(s.find_last_not_of(ws) + 1);
        };

    while (std::getline(in, Line)) {
        // 주석(# 또는 ;)과 빈 줄 건너뛰기
        if (Line.empty() || Line[0] == '#' || Line[0] == ';')
            continue;

        // 섹션 식별
        if (Line.front() == '[' && Line.back() == ']') {
            bInSection = (Line == SectionTag);
            continue;
        }
        if (!bInSection)
            continue;

        // key=value 파싱
        auto pos = Line.find('=');
        if (pos == std::string::npos)
            continue;
        std::string key = Line.substr(0, pos);
        std::string value = Line.substr(pos + 1);
        trim(key);
        trim(value);
        KeyValueMap[key] = value;
    }

    return KeyValueMap;
}

}// namespace util
