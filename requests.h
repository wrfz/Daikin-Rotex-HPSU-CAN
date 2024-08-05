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

const std::vector<std::vector<uint8_t>> data_requests = {
    {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00}, // Status Kessel
    {0x31, 0x00, 0xFA, 0xC0, 0xF7, 0x00, 0x00}, // Umwälzpumpe
    {0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0xC0, 0xF6, 0x00, 0x00},
    {0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0xC0, 0xFB, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x01, 0xDA, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0xA4, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0xC1, 0x02, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0xC0, 0xFC, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0xC1, 0x00, 0x00, 0x00}, // Rücklauftemperatur Hzg - ok 1/10
    {0x31, 0x00, 0xfa, 0x00, 0x0e, 0x00, 0x00},
    {0x31, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00}, // Warmwasser Temperatur  °C et dec value - ok /10
    {0x31, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0xA5, 0x00, 0x00},
    {0x31, 0x00, 0xfa, 0x01, 0x2B, 0x00, 0x00},
    {0x31, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x01, 0x41, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x01, 0x29, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x13, 0x88, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x09, 0x30, 0x00, 0x00},
    {0x31, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x09, 0x20, 0x00, 0x00}, // Qchhp
    {0x31, 0x00, 0xFA, 0xC0, 0xFF, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x01, 0x0E, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0x83, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0x84, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0x94, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0x93, 0x00, 0x00},
    {0xA1, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00},
    {0x31, 0x00, 0xFA, 0x06, 0x9B, 0x00, 0x00}, // Status Mischer DHW
    {0x31, 0x00, 0xFA, 0x0A, 0x20, 0x00, 0x00}  // Status Heizstäbe
};

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

uint32_t request_index = 0;

void sendRequest(esphome::esp32_can::ESP32Can* can_bus)
{
    if (request_index >= data_requests.size()) {
        request_index = 0;
    }

    const std::vector<uint8_t>& data = data_requests[request_index];
    const uint32_t can_id = 0x680;
    const bool use_extended_id = false;

    can_bus->send_data(can_id, use_extended_id, data);
    ESP_LOG_FILTER("main", "request(%d, %d)", request_index++, data);
}

void onFrame(esphome::esp32_can::ESP32Can* can_bus, uint32_t can_id, const std::vector<uint8_t>& data)
{
    ESP_LOG_FILTER("onHandle", "can_id<%s> data<%s>", ::to_hex(can_id).c_str(), ::to_hex(data).c_str());

    switch (can_id)
    {
    case 0x180:

        // Mischer DHW Position
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0x9B) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(dhw_mischer_postion).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Fehlercode
        if (data[2] == 0xFA and data[3] == 0x13 and data[4] == 0x88) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Fehlercode).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Heizkurve
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x0E) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/100));
            id(heizkurve).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Status Kesselpumpe
        if (data[2] == 0xFA and data[3] == 0x0A and data[4] == 0x8C) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(status_kessel).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Wasserdruck
        if (data[0] == 0xd2 and data[2] == 0x1C) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/1000));
            id(Wasserdruck).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Qchhp
        if (data[2] == 0xFA and data[3] == 0x09 and data[4] == 0x20) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Qchhp).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Erzeugte Energie gesamt
        if (data[2] == 0xFA and data[3] == 0x09 and data[4] == 0x30) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Erzeugte_Energie_Gesamt).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Umwälzpumpe
        if (data[2] == 0xFA and data[3] == 0xC0 and data[4] == 0xF7) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Umwaelzpumpe).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Betriebsart
        if (data[2] == 0xFA and data[3] == 0xC0 and data[4] == 0xF6) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsart).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Betriebsmodus
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x12) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsmodus).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // VL Soll
        if (data[0] == 0xd2 and data[2] == 0x02) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // BPV
        if (data[2] == 0xFA and data[3] == 0xC0 and data[4] == 0xFB) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(BPV).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Volumenstrom
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0xDA) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(durchfluss).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // rt_pump
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0xA4) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_pump).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // rt_compressor
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0xA5) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_compressor).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Warmwasser-Temperaturabfrage
        if (data[2] == 0xFA and data[3] == 0x00 and data[4] == 0x0E) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(temperature_water).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Vorlauftemperaturabfrage (TVBH)
        if (data[2] == 0xFA and data[3] == 0xC1 and data[4] == 0x02) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TVBH).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Vorlauftemperaturabfrage Heizkreis (TV)
        if (data[2] == 0xFA and data[3] == 0xC0 and data[4] == 0xFC) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TV).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Rücklauftemperaturabfrage
        if (data[2] == 0xFA and data[3] == 0xC1 and data[4] == 0x00) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(ruecklauf).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Außentemperaturabfrage
        if (data[2] == 0xFA and data[3] == 0xC0 and data[4] == 0xFF) {
            float temperature = float(((int((data[6]) + ((data[5]) << 8))) ^ 0x8000) - 0x8000)/10;
            id(temperature_outside).publish_state(temperature); 
            ESP_LOG_FILTER("main", "Aussentemperatur: %f", temperature);
        }

        // HK Funktion
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x41) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(hk_funktion).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Spreizung MOD HZ
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0x83) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_hz).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Spreizung MOD WW
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0x84) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_ww).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Spreizung MOD WW
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x29) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(t_vorlauf_tag).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // SG Modus
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0x94) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(SGModus).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Smart Grid
        if (data[2] == 0xFA and data[3] == 0x06 and data[4] == 0x93) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Smartgrid).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Raumsoll
        if (data[0] == 0xd2 and data[1] == 0x00 and data[2] == 0x05 and data[3] == 0x00 and data[5] == 0x00) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(raumsoll1).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Warmwasser
        if (data[0] == 0xd2 && data[1] == 0x00 && data[2] == 0x13 && data[5] == 0x00 && data[6] == 0x00) {
            uint16_t temperature_raw = (data[3] <<  8) | data[4]; // Convert to int16be
            float temperature = static_cast<float>(temperature_raw) / 10.0;
            id(t_ww_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Min Vl Soll
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x2B) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(min_vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Max Vl Soll
        if (data[2] == 0x28) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(max_vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // Heizstabrelais
        if (data[2] == 0xFA and data[3] == 0x0A and data[4] == 0x20) {
            const bool r1 = (data[5] == 0) && (data[6] & 0x40) == 0x40;
            const bool r2 = (data[5] == 0) && (data[6] & 0x80) == 0x80;
            const bool r3 = (data[5] == 1) && (data[6] == 0);
            id(status_heizstab).publish_state((0x4 * r1) | (0x2 * r2) | r3);
            ESP_LOGD("main", "Heating element states: %d|%d|%d status: %d", r1, r2, r3, status_heizstab);
        }

        // Betriebsmodus (Automatik bei jeglicher Änderung)0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00
        if (data[2] == 0xFA and data[3] == 0x01 and data[4] == 0x12)
        {
            if (data[5] == 0x01) {
                auto call = id(betrieb).make_call();
                call.set_option("Bereitschaft");
                call.perform();
            } else if (data[5] == 0x03) {
                auto call = id(betrieb).make_call();
                call.set_option("Heizen");
                call.perform();
            } else if (data[5] == 0x04) {
                auto call = id(betrieb).make_call();
                call.set_option("Absenken");
                call.perform();
            } else if (data[5] == 0x05) {
                auto call = id(betrieb).make_call();
                call.set_option("Sommer");
                call.perform();
            } else if (data[5] == 0x17) {
                auto call = id(betrieb).make_call();
                call.set_option("Kühlen");
                call.perform();
            } else if (data[5] == 0x0B) {
                auto call = id(betrieb).make_call();
                call.set_option("Automatik 1");
                call.perform();
            } else if (data[5] == 0x0C) {
                auto call = id(betrieb).make_call();
                call.set_option("Automatik 2");
                call.perform();
            }
        }
        break;

    case 0x500:
        // Status Kompressor
        if (data[2] == 0x61) {
            float temperature = float((float((int((data[3]) + ((data[4]) << 8))))));
            id(status_kompressor).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }
        break;
    }

    //sendRequest(can_bus);
}
