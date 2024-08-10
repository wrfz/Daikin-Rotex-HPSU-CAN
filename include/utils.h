#pragma once

#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdint.h>
#include <memory>

class Utils {
public:
    template<typename... Args>
    static std::string str_format(const std::string& format, Args... args)
    {
        const auto size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
        const auto buffer = std::make_unique<char[]>(size);

        std::snprintf(buffer.get(), size, format.c_str(), args...);

        return std::string(buffer.get(), buffer.get() + size - 1);
    }

    static std::vector<std::string> split(std::string const& str);
    static bool is_number(const std::string& str);
    static std::string to_hex(uint32_t value);

    template<typename T>
    static std::string to_hex(const T& data)
    {
        std::stringstream str;
        str.setf(std::ios_base::hex, std::ios::basefield);
        str.setf(std::ios_base::uppercase);
        str.fill('0');

        bool first = true;
        for (uint8_t chr : data)
        {
            if (first) {
                first = false;
            } else {
                str << " ";
            }
            str << "0x" << std::setw(2) << (unsigned short)(std::byte)chr;
        }
        return str.str();
    }

};

#define ESP_LOG_FILTER(tag, format, ...)                            \
    const std::string tag_str = std::string(tag);                   \
    const std::string formated = Utils::str_format(format, __VA_ARGS__);   \
    const std::string log_filter = id(log_filter_text).state;       \
    bool found = log_filter.empty();                                \
    if (!found) {                                                   \
        for (auto segment : Utils::split(log_filter)) {                   \
            if (tag_str.find(segment) != std::string::npos || formated.find(segment) != std::string::npos) { \
                found = true;                                       \
                break;                                              \
            }                                                       \
        }                                                           \
    }                                                               \
    if (found) ESP_LOGI(tag, formated.c_str())
