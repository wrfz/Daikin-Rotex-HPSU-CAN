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
}
