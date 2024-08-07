const uint16_t DC = 0xFFFF; // Don't care

class TRequest
{
public:
    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        uint32_t response_can_id,
        std::array<uint16_t, 7> const& expected_reponse,
        std::function<void(std::vector<uint8_t> const&)> lambda)
    : m_name(name)
    , m_data(data)
    , m_response_can_id(response_can_id)
    , m_expected_reponse(expected_reponse)
    , m_lambda(lambda)
    , m_lambda_used(true)
    {
    }

    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        std::array<uint16_t, 7> const& expected_reponse,
        std::function<void(std::vector<uint8_t> const&)> lambda)
    : TRequest(name, data, 0x180, expected_reponse, lambda)
    {
    }

    std::array<uint8_t, 7> data() const
    {
        return m_data;
    }

    bool startsWith(std::vector<uint8_t> const& data) const {
        //ESP_LOGI("startsWith", "data<%s>, expected<%s>", to_hex(data).c_str(), to_hex(m_expected_reponse).c_str());
        for (uint32_t index = 0; index < data.size(); ++index) {
            if (m_expected_reponse[index] != DC && data[index] != m_expected_reponse[index]) {
                return false;
            }
        }
        //ESP_LOGI("< startsWith", "match");
        return true;
    }

    bool handle(uint32_t can_id, std::vector<uint8_t> const& data) const {
        if (m_lambda_used && can_id == m_response_can_id && startsWith(data)) {
            //ESP_LOGI("handle", "!!!!!!!! found !!!!!!!!");
            m_lambda(data);
            return true;
        }
        return false;
    }

    std::string const& getName() const {
        return m_name;
    }
private:
    std::string m_name;
    std::array<uint8_t, 7> m_data;
    uint32_t m_response_can_id;
    std::array<uint16_t, 7> m_expected_reponse;
    std::function<void(std::vector<uint8_t> const&)> m_lambda;
    bool m_lambda_used;
};

class TRequests {
public:
    TRequests(std::vector<TRequest> const& requests)
    : m_requests(requests)
    {
    }

    uint32_t size() const {
        return m_requests.size();
    }

    TRequest const& get(uint32_t index) const {
        return m_requests[index];
    }

    bool handle(esphome::esp32_can::ESP32Can* can_bus, uint32_t can_id, std::vector<uint8_t> const& data) const {
        ESP_LOG_FILTER("handle", "can_id<%s> data<%s>", ::to_hex(can_id).c_str(), ::to_hex(data).c_str());
        for (auto const& request : m_requests) {
            if (request.handle(can_id, data)) {
                return true;
            }
        }
        return false;
    }
private:
    std::vector<TRequest> m_requests;
};

TRequests data_requests({
    {
        "Status Kessel",
        {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
        [](auto const& data) {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(status_kessel).publish_state(status);
            ESP_LOG_FILTER("main", "Status Kessel: %f", status);
        }
    },
    {
        "Umwälzpumpe",
        {0x31, 0x00, 0xFA, 0xC0, 0xF7, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xF7,   DC,   DC},
        [](auto const& data) {
            float status = float((float((int((data[6]) + ((data[5])))))));
            id(Umwaelzpumpe).publish_state(status);
            ESP_LOG_FILTER("main", "Umwälzpumpe: %f", status);
        }
    },
    {
        "Betriebsmodus",
        {0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x12,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsmodus).publish_state(temperature);
            ESP_LOG_FILTER("main", "Betriebsmodus: %f", temperature);

            // Betriebsmodus (Automatik bei jeglicher Änderung)0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00

            const std::map<uint8_t, std::string> map_betriebsmodus {
                {0x01, "Bereitschaft"},
                {0x03, "Heizen"},
                {0x04, "Absenken"},
                {0x05, "Sommer"},
                {0x17, "Kühlen"},
                {0x0B, "Automatik 1"},
                {0x0C, "Automatik 2"}
            };
            auto const iter = map_betriebsmodus.find(data[5]);
            if (iter != map_betriebsmodus.end()) {
                auto call = id(betrieb).make_call();
                call.set_option(iter->second);
                call.perform();
            }
        }
    },
    {
        "Betriebsart",
        {0x31, 0x00, 0xFA, 0xC0, 0xF6, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xF6,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsart).publish_state(temperature);
            ESP_LOG_FILTER("main", "Betriebsart: %f", temperature);
        }
    },
    {
        "VL Soll",
        {0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00},
        {0xD2,   DC, 0x02,   DC,   DC,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "VL Soll: %f", temperature);
        }
    },
    {
        "BPV",
        {0x31, 0x00, 0xFA, 0xC0, 0xFB, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFB,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(BPV).publish_state(temperature);
            ESP_LOG_FILTER("main", "BPV: %f", temperature);
        }
    },
    {
        "Durchfluss",
        {0x31, 0x00, 0xFA, 0x01, 0xDA, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0xDA,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(durchfluss).publish_state(temperature);
            ESP_LOG_FILTER("main", "Volumenstrom: %f", temperature);
        }
    },
    {
        "Laufzeit Pumpe",
        {0x31, 0x00, 0xFA, 0x06, 0xA4, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0xA4,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_pump).publish_state(temperature);
            ESP_LOG_FILTER("main", "Laufzeit Pump: %f", temperature);
        }
    },
    {
        "Vorlauftemperatur (TVBH)",
        {0x31, 0x00, 0xFA, 0xC1, 0x02, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC1, 0x02,   DC,   DC},
        [](auto const& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TVBH).publish_state(temperature);
            ESP_LOG_FILTER("main", "Vorlauftemperatur: %f", temperature);
        }
    },
    {
        "Vorlauftemperatur",
        {0x31, 0x00, 0xFA, 0xC0, 0xFC, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFC,   DC,   DC},
        [](auto const& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TV).publish_state(temperature);
            ESP_LOG_FILTER("main", "Vorlauftemperatur: %f", temperature);
        }
    },
    {
        "Rücklauftemperatur Heizung",
        {0x31, 0x00, 0xFA, 0xC1, 0x00, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC1, 0x00,   DC,   DC},
        [](auto const& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(ruecklauf).publish_state(temperature);
            ESP_LOG_FILTER("main", "Rücklauftemperatur: %f", temperature);
        }
    },
    {
        "Warmwassertemperatur",
        {0x31, 0x00, 0xfa, 0x00, 0x0e, 0x00, 0x00},
        {  DC,   DC, 0xfa, 0x00, 0x0e,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(temperature_water).publish_state(temperature);
            ESP_LOG_FILTER("main", "Warmwassertemperatur: %f", temperature);
        }
    },
    {
        "Warmwasser",
        {0x31, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00},
        {0xd2, 0x00,   DC,   DC,   DC, 0x00, 0x00},
        [](auto const& data) {
            uint16_t temperature_raw = (data[3] <<  8) | data[4]; // Convert to int16be
            float temperature = static_cast<float>(temperature_raw) / 10.0;
            id(t_ww_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Warmwasser soll: %f", temperature);
        }
    },
    {
        "zzz",
        {0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00},
        {0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00},
        [](auto const& data) {
        }
    }, // Warmwasser Temperatur  °C et dec value - ok /10
    {
        "Raumsoll",
        {0x31, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00},
        {0xD2, 0x00, 0x05, 0x00,   DC, 0x00,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(raumsoll1).publish_state(temperature);
            ESP_LOG_FILTER("main", "Raumsoll: %f", temperature);
        }
    },
    {
        "Laufzeit Kompressor",
        {0x31, 0x00, 0xFA, 0x06, 0xA5, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0xA5,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_compressor).publish_state(temperature);
            ESP_LOG_FILTER("main", "Laufzeit Kompressor: %f", temperature);
        }
    },
    {
        "Min Vl Soll",
        {0x31, 0x00, 0xfa, 0x01, 0x2B, 0x00, 0x00},
        {  DC,   DC, 0xfa, 0x01, 0x2B,   DC,   DC},
        [](auto const& data) {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(min_vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Min Vl Soll: %f", temperature);
        }
    },
    {
        "Max Vl Soll",
        {0x31, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00},
        {  DC,   DC, 0x28,   DC,   DC,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(max_vl_soll).publish_state(temperature);
            ESP_LOG_FILTER("main", "Max Vl Soll: %f", temperature);
        }
    },
    {
        "HK Funktion",
        {0x31, 0x00, 0xFA, 0x01, 0x41, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x41,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(hk_funktion).publish_state(temperature);
            ESP_LOG_FILTER("main", "HK Funktion: %f", temperature);
        }
    },
    {
        "Temperatur Vorlauf Tag",
        {0x31, 0x00, 0xFA, 0x01, 0x29, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x29,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(t_vorlauf_tag).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperatur Vorlauf Tag: %f", temperature);
        }
    },
    {
        "Fehlercode",
        {0x31, 0x00, 0xFA, 0x13, 0x88, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x13, 0x88,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Fehlercode).publish_state(temperature);
            ESP_LOG_FILTER("main", "Fehlercode: %f", temperature);
        }
    },
    {
        "Erzeugte Energie gesamt",
        {0x31, 0x00, 0xFA, 0x09, 0x30, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x09, 0x30,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Erzeugte_Energie_Gesamt).publish_state(temperature);
            ESP_LOG_FILTER("main", "Erzeugte Energie gesamt: %f", temperature);
        }
    },
    {
        "Wasserdruck",
        {0x31, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00},
        {0xD2,   DC, 0x1C,   DC,   DC,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/1000));
            id(Wasserdruck).publish_state(temperature);
            ESP_LOG_FILTER("main", "Wasserdruck: %f", temperature);
        }
    },
    {
        "EHS für CH",
        {0x31, 0x00, 0xFA, 0x09, 0x20, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x09, 0x20,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Qchhp).publish_state(temperature);
            ESP_LOG_FILTER("main", "EHS fuer CH: %f", temperature);
        }
    },
    {
        "Außentemperatur",
        {0x31, 0x00, 0xFA, 0xC0, 0xFF, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFF,   DC,   DC},
        [](auto const& data) {
            float temperature = float(((int((data[6]) + ((data[5]) << 8))) ^ 0x8000) - 0x8000)/10;
            id(temperature_outside).publish_state(temperature); 
            ESP_LOG_FILTER("main", "Außentemperatur: %f", temperature);
        }
    },
    {
        "Heizkurve",
        {0x31, 0x00, 0xFA, 0x01, 0x0E, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x0E,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/100));
            id(heizkurve).publish_state(temperature);
            ESP_LOG_FILTER("main", "Heizkurve: %f", temperature);
        }
    },
    {
        "Spreizung MOD HZ",
        {0x31, 0x00, 0xFA, 0x06, 0x83, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x83,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_hz).publish_state(temperature);
            ESP_LOG_FILTER("main", "Spreizung MOD HZ: %f", temperature);
        }
    },
    {
        "Spreizung MOD WW",
        {0x31, 0x00, 0xFA, 0x06, 0x84, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x84,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_ww).publish_state(temperature);
            ESP_LOG_FILTER("main", "Spreizung MOD WW: %f", temperature);
        }
    },
    {
        "SGModus",
        {0x31, 0x00, 0xFA, 0x06, 0x94, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x94,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(SGModus).publish_state(temperature);
            ESP_LOG_FILTER("main", "SGModus: %f", temperature);
        }
    },
    {
        "Smart Grid",
        {0x31, 0x00, 0xFA, 0x06, 0x93, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x93,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Smartgrid).publish_state(temperature);
            ESP_LOG_FILTER("main", "Smart Grid: %f", temperature);
        }
    },
    {
        "Status Kompressor",
        {0xA1, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00},
        0x500,
        {  DC,   DC, 0x61,   DC,   DC,   DC,   DC},
        [](auto const& data) {
            float temperature = float((float((int((data[3]) + ((data[4]) << 8))))));
            id(status_kompressor).publish_state(temperature);
            ESP_LOG_FILTER("main", "Status Kompressor: %f", temperature);
        }
    },
    {
        "Status Mischer DHW",
        {0x31, 0x00, 0xFA, 0x06, 0x9B, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x9B,   DC,   DC},
        [](auto const& data) {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(dhw_mischer_postion).publish_state(status);
            ESP_LOG_FILTER("main", "Status Mischer DHW: %f", status);
        }
    },
    {
        "Heizstabrelais",
        {0x31, 0x00, 0xFA, 0x0A, 0x20, 0x00, 0x00},
        {0xD2, 0x00, 0xFA, 0x0A, 0x20, DC, DC},
        [](auto const& data) {
            const bool r1 = (data[5] == 0) && (data[6] & 0x40) == 0x40;
            const bool r2 = (data[5] == 0) && (data[6] & 0x80) == 0x80;
            const bool r3 = (data[5] == 1) && (data[6] == 0);
            id(status_heizstab).publish_state((0x4 * r1) | (0x2 * r2) | r3);
            ESP_LOG_FILTER("main", "Heizstabrelais: %d|%d|%d status: %d", r1, r2, r3, status_heizstab);
        }
    }  // Status Heizstäbe
});

uint32_t request_index = 0;

void sendRequest(esphome::esp32_can::ESP32Can* can_bus)
{
    if (request_index >= data_requests.size()) {
        request_index = 0;
    }

    TRequest const& request = data_requests.get(request_index);
    const uint32_t can_id = 0x680;
    const bool use_extended_id = false;
    std::array<uint8_t, 7> const& data = request.data();

    can_bus->send_data(can_id, use_extended_id, { data.begin(), data.end() });
    ESP_LOG_FILTER("main", "request(%d, %s, %s)", request_index++, request.getName().c_str(), to_hex(request.data()).c_str());
}
