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

bool split(std::string const& str, std::function<bool(std::string const&)> lambda)
{
    std::string segment;
    std::istringstream iss(str);

    bool found = str.empty();
    while (!found && std::getline(iss, segment, '|'))
    {
        found = lambda(segment);
    }

    return found;
}

std::vector<std::string> split2(std::string const& str)
{
    std::string segment;
    std::istringstream iss(str);
    std::vector<std::string> result;

    while (std::getline(iss, segment, '|'))
    {
        result.push_back(segment);
    }

    return result;
}

//    auto lambda = [&tag_str, &formated](auto segment) {
//        return tag_str.find(segment) != std::string::npos || formated.find(segment) != std::string::npos;
//    };

#define ESP_LOG_FILTER(tag, format, ...)                            \
    const std::string tag_str = std::string(tag);                   \
    const std::string formated = str_printf(format, __VA_ARGS__);   \
    const std::string log_filter = id(log_filter_text).state;       \
    bool found = log_filter.empty();                                \
    if (!found) {                                                   \
        for (auto segment : split2(log_filter)) {                   \
            if (tag_str.find(segment) != std::string::npos || formated.find(segment) != std::string::npos) { \
                found = true;                                       \
                break;                                              \
            }                                                       \
        }                                                           \
    }                                                               \
    if (found) ESP_LOGI(tag, formated.c_str())

bool is_number(const std::string& str)
{
    return !str.empty() && std::find_if(
        str.begin(), 
        str.end(),
        [](unsigned char chr) { return !std::isdigit(chr); }
    ) == str.end();
}

std::string to_hex(uint32_t value)
{
    char hex_string[20];
    sprintf(hex_string, "0x%X", value);
    return std::string(hex_string);
}

template<class T>
std::string to_hex(const T& data)
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
