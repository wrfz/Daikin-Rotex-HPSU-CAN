#pragma once

#define ESP_LOGI(tag, ...)


struct TextEntity {
    std::string state;
};
struct NumberEntity {
    float state;
};

namespace esphome {
namespace template_ {

    struct TemplateText {};
    struct TemplateNumber {};
}

namespace esp32_can {

class ESP32Can {
public:
    virtual void send_data(uint32_t can_id, bool use_extended_id, const std::vector<uint8_t>& data) {}
};

}
}

esphome::template_::TemplateText* log_filter_text;
esphome::template_::TemplateNumber* update_interval;

TextEntity id(esphome::template_::TemplateText* templateText) {
    return TextEntity();
}
NumberEntity id(esphome::template_::TemplateNumber* templateNumber) {
    return NumberEntity();
}

uint32_t g_millis = 0;
uint32_t millis() {
    return g_millis;
}
