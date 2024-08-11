#include "dummys.h"

TextEntity id(esphome::template_::TemplateText* templateText) {
    return TextEntity();
}
NumberEntity id(esphome::template_::TemplateNumber* templateNumber) {
    return NumberEntity();
}

uint32_t millis() {
    return g_millis;
}

esphome::template_::TemplateText* log_filter_text;
esphome::template_::TemplateNumber* update_interval;

uint32_t g_millis = 0;