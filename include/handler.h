void onFrame(esphome::esp32_can::ESP32Can* can_bus, uint32_t can_id, const std::vector<uint8_t>& data)
{
    ESP_LOG_FILTER("onHandle", "can_id<%s> data<%s>", ::to_hex(can_id).c_str(), ::to_hex(data).c_str());

    switch (can_id)
    {
    case 0x180:
        if (data_requests.handle(data)) {
            return;
        }

        // Wasserdruck
        if (data[0] == 0xd2 and data[2] == 0x1C) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/1000));
            id(Wasserdruck).publish_state(temperature);
            ESP_LOG_FILTER("main", "Temperature received over can is %f", temperature);
        }

        // VL Soll
        if (data[0] == 0xd2 and data[2] == 0x02) {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(vl_soll).publish_state(temperature);
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
}
