#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#define ESP_LOGI(tag, ...)


struct TextEntity {
    std::string state;
};
struct NumberEntity {
    float state;
};

namespace esphome {
namespace template_ {

    class NumberCall {
    public:
        void set_option(const std::string);
        void perform();
    };

    class TemplateText {};
    class TemplateNumber {};
    class TemplateSelect {
    public:
        NumberCall make_call();
    };
}

namespace esp32_can {

class ESP32Can {
public:
    virtual void send_data(uint32_t can_id, bool use_extended_id, const std::vector<uint8_t>& data) {}
};

}
}


TextEntity id(esphome::template_::TemplateText* templateText);
NumberEntity id(esphome::template_::TemplateNumber* templateNumber);
uint32_t millis();

extern esphome::template_::TemplateText* log_filter_text;
extern esphome::template_::TemplateNumber* update_interval;
extern uint32_t g_millis;
