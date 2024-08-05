#include <sstream>
#include <iomanip>
#include <limits>

const uint32_t MAX_UINT32 = std::numeric_limits<uint32_t>::max();

std::string str_printf (const char* format, ...)
{
    std::va_list args;
    std::string retval;
    va_start(args, format);
    retval.resize (vsnprintf(0, 0, format, args));
    vsnprintf(&retval[0], retval.size () + 1, format, args);
    va_end(args);

    return retval;
}

#define ESP_LOG_FILTER(tag, format, ...)                            \
    const std::string tag_str = std::string(tag);                   \
    const std::string formated = str_printf(format, __VA_ARGS__);   \
    const std::string log_filter = id(log_filter_text).state;       \
    if (log_filter == "" || tag_str.find(log_filter) != std::string::npos || formated.find(log_filter) != std::string::npos) ESP_LOGI(tag, formated.c_str())

std::string to_hex(uint32_t value)
{
    char hex_string[20];
    sprintf(hex_string, "0x%X", value);
    return std::string(hex_string);
}

std::string to_hex(const std::vector<uint8_t>& data)
{
    std::stringstream str;
    str.setf(std::ios_base::hex, std::ios::basefield);
    str.setf(std::ios_base::uppercase);
    str.fill('0');

    bool first = true;
    for (uint8_t chr : data){
        if (first) {
            first = false;
        } else {
            str << " ";
        }
        str << "0x" << std::setw(2) << (unsigned short)(byte)chr;
    }
    return str.str();
}
