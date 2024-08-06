class TRequest
{
public:
    TRequest(uint8_t bitmask, const std::vector<uint8_t>& data, std::function<void(const std::vector<uint8_t>&)> lambda)
    : m_data(data)
    , m_lambda(lambda)
    , m_lambda_used(true)
    , m_bitmask(bitmask)
    {
    }

    TRequest(const std::vector<uint8_t>& data, std::function<void(const std::vector<uint8_t>&)> lambda)
    : m_data(data)
    , m_lambda(lambda)
    , m_lambda_used(true)
    , m_bitmask(0b0011100)
    {
    }

    TRequest(const std::vector<uint8_t>& data)
    : m_data(data)
    , m_lambda_used(false)
    , m_bitmask(0b0011100)
    {
    }

    std::vector<uint8_t> data() const
    {
        return m_data;
    }

    bool startsWith(const std::vector<uint8_t>& data) const {
        //ESP_LOGI("> startsWith", "m_bitmask: %d", m_bitmask);
        for (uint32_t index = 0; index < data.size(); ++index) {
            const uint32_t bit_to_check = 1 << (7 - index - 1);
            const bool bCompare = (m_bitmask & bit_to_check) == bit_to_check;
            //ESP_LOGI("  startsWith", "index: %d, bit_to_check: %d, bCompare: %d", index, bit_to_check, bCompare);
            if (bCompare) {
                //ESP_LOGI("  startsWith", "data1: %d, data: %d", m_data[index], data[index]);
                if (m_data[index] != data[index]) {
                    return false;
                }
            }
        }
        //ESP_LOGI("< startsWith", "ok");
        return true;
    }

    bool handle(const std::vector<uint8_t>& data) const {
        if (m_lambda_used && startsWith(data)) {
            //ESP_LOGI("handle", "!!!!!!!! found !!!!!!!!");
            m_lambda(m_data);
            return true;
        }
        return false;
    }
private:
    std::vector<uint8_t> m_data;
    std::function<void(const std::vector<uint8_t>&)> m_lambda;
    bool m_lambda_used;
    uint8_t m_bitmask;
};

class TRequests {
public:
    TRequests(const std::vector<TRequest>& requests)
    : m_requests(requests)
    {
    }

    uint32_t size() const {
        return m_requests.size();
    }

    const TRequest& get(uint32_t index) const {
        return m_requests[index];
    }

    bool handle(const std::vector<uint8_t>& data) const {
        //ESP_LOGI("TRequests::handle", "size: %d", m_requests.size());
        for (auto const& request : m_requests) {
            if (request.handle(data)) {
                return true;
            }
        }
        return false;
    }
private:
    std::vector<TRequest> m_requests;
};

TRequests data_requests({
    // Status Kessel
    {
        {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
        [](const auto& data) {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(status_kessel).publish_state(status);
            ESP_LOG_FILTER("main", "Status Kessel: %f", status);
        }
    },
    // Umwälzpumpe
    {
        {0x31, 0x00, 0xFA, 0xC0, 0xF7, 0x00, 0x00},
        [](const auto& data) {
            float status = float((float((int((data[6]) + ((data[5])))))));
            id(Umwaelzpumpe).publish_state(status);
            ESP_LOG_FILTER("main", "Umwälzpumpe: %f", status);
        }
    },
    // Betriebsart
    {{0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00}},
    {
        {0x31, 0x00, 0xFA, 0xC0, 0xF6, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsart).publish_state(temperature);
            ESP_LOG_FILTER("main", "Betriebsart: %f", temperature);
        }
    },
    // BPV
    {{0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00}},
    {
        {0x31, 0x00, 0xFA, 0xC0, 0xFB, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(BPV).publish_state(temperature);
            ESP_LOG_FILTER("main", "BPV: %f", temperature);
        }
    },
    // Volumenstrom
    {
        {0x31, 0x00, 0xFA, 0x01, 0xDA, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(durchfluss).publish_state(temperature);
            ESP_LOG_FILTER("main", "Volumenstrom: %f", temperature);
        }
    },
    // Laufzeit Pump
    {
        {0x31, 0x00, 0xFA, 0x06, 0xA4, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_pump).publish_state(temperature);
            ESP_LOG_FILTER("main", "Laufzeit Pump: %f", temperature);
        }
    },
    // Vorlauftemperaturabfrage (TVBH)
    {
        {0x31, 0x00, 0xFA, 0xC1, 0x02, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TVBH).publish_state(temperature);
            ESP_LOG_FILTER("main", "Vorlauftemperatur: %f", temperature);
        }
    },
    // Vorlauftemperaturabfrage Heizkreis (TV)
    {
        {0x31, 0x00, 0xFA, 0xC0, 0xFC, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TV).publish_state(temperature);
            ESP_LOG_FILTER("main", "Vorlauftemperatur: %f", temperature);
        }
    },
    // Rücklauftemperatur Heizung
    {
        {0x31, 0x00, 0xFA, 0xC1, 0x00, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(ruecklauf).publish_state(temperature);
            ESP_LOG_FILTER("main", "Rücklauftemperatur: %f", temperature);
        }
    },
    // Warmwasser-Temperaturabfrage
    {
        {0x31, 0x00, 0xfa, 0x00, 0x0e, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(temperature_water).publish_state(temperature);
            ESP_LOG_FILTER("main", "Warmwassertemperatur: %f", temperature);
        }
    },
    {{0x31, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00}},
    {{0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00}}, // Warmwasser Temperatur  °C et dec value - ok /10
    {{0x31, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00}},
    // Laufzeit Kompressor
    {
        {0x31, 0x00, 0xFA, 0x06, 0xA5, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_compressor).publish_state(temperature);
            ESP_LOG_FILTER("main", "Laufzeit Kompressor: %f", temperature);
        }
    },
    {{0x31, 0x00, 0xfa, 0x01, 0x2B, 0x00, 0x00}},
    {{0x31, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00}},
    {{0x31, 0x00, 0xFA, 0x01, 0x41, 0x00, 0x00}},
    {{0x31, 0x00, 0xFA, 0x01, 0x29, 0x00, 0x00}},
    // Fehlercode
    {
        {0x31, 0x00, 0xFA, 0x13, 0x88, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Fehlercode).publish_state(temperature);
            ESP_LOG_FILTER("main", "Fehlercode: %f", temperature);
        }
    },
    // Erzeugte Energie gesamt
    {
        {0x31, 0x00, 0xFA, 0x09, 0x30, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Erzeugte_Energie_Gesamt).publish_state(temperature);
            ESP_LOG_FILTER("main", "Erzeugte Energie gesamt: %f", temperature);
        }
    },
    {{0x31, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00}},
    // EHS für CH
    {
        {0x31, 0x00, 0xFA, 0x09, 0x20, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Qchhp).publish_state(temperature);
            ESP_LOG_FILTER("main", "EHS fuer CH: %f", temperature);
        }
    },
    {{0x31, 0x00, 0xFA, 0xC0, 0xFF, 0x00, 0x00}},
    // Heizkurve
    {
        {0x31, 0x00, 0xFA, 0x01, 0x0E, 0x00, 0x00},
        [](const auto& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/100));
            id(heizkurve).publish_state(temperature);
            ESP_LOG_FILTER("main", "Heizkurve: %f", temperature);
        }
    },
    {{0x31, 0x00, 0xFA, 0x06, 0x83, 0x00, 0x00}},
    {{0x31, 0x00, 0xFA, 0x06, 0x84, 0x00, 0x00}},
    {{0x31, 0x00, 0xFA, 0x06, 0x94, 0x00, 0x00}},
    {{0x31, 0x00, 0xFA, 0x06, 0x93, 0x00, 0x00}},
    {{0xA1, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00}},
    // Status Mischer DHW
    {
        {0x31, 0x00, 0xFA, 0x06, 0x9B, 0x00, 0x00},
        [](const auto& data) {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(dhw_mischer_postion).publish_state(status);
            ESP_LOG_FILTER("main", "Status Mischer DHW: %f", status);
        }
    },
    {{0x31, 0x00, 0xFA, 0x0A, 0x20, 0x00, 0x00}}  // Status Heizstäbe
});

uint32_t request_index = 0;

void sendRequest(esphome::esp32_can::ESP32Can* can_bus)
{
    if (request_index >= data_requests.size()) {
        request_index = 0;
    }

    const TRequest& data = data_requests.get(request_index);
    const uint32_t can_id = 0x680;
    const bool use_extended_id = false;

    can_bus->send_data(can_id, use_extended_id, data.data());
    ESP_LOG_FILTER("main", "request(%d, %d)", request_index++, data);
}
