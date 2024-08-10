#include "utils.h"
#include <algorithm>

std::vector<std::string> Utils::split(std::string const& str)
{
    std::string segment;
    std::istringstream iss(str);
    std::vector<std::string> result;

    while (std::getline(iss, segment, '|'))
    {
        if (!segment.empty()) {
            result.push_back(segment);
        }
    }

    return result;
}

bool Utils::is_number(const std::string& str)
{
    return !str.empty() && std::find_if(
        str.begin(), 
        str.end(),
        [](unsigned char chr) { return !std::isdigit(chr); }
    ) == str.end();
}

std::string Utils::to_hex(uint32_t value)
{
    char hex_string[20];
    sprintf(hex_string, "0x%02X", value);
    return std::string(hex_string);
}