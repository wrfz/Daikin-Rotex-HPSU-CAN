#pragma once

#include "request.h"

static const BidiMap<uint8_t, std::string> map_betriebsmodus {
    {0x01, "Bereitschaft"},
    {0x03, "Heizen"},
    {0x04, "Absenken"},
    {0x05, "Sommer"},
    {0x11, "Kühlen"},
    {0x0B, "Automatik 1"},
    {0x0C, "Automatik 2"}
};

static const BidiMap<uint8_t, std::string> map_hk = {
    {0x00, "Witterungsgeführt"},
    {0x01, "Fest"}
};

static const BidiMap<uint8_t, std::string> map_sg_mode = {
    {0x00, "Aus"},
    {0x01, "SG Modus 1"},
    {0x02, "SG Modus 2"}
};

static const BidiMap<uint8_t, std::string> map_sg = {
    {0x00, "Aus"},
    {0x01, "An"}
};

const std::vector<TRequest> entity_config = {
    {// 0
        "Status Kessel",
        {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
        [](auto const& data) -> std::string {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(status_kessel).publish_state(status);
            return Utils::str_format("Status Kessel: %f", status);
        }
    },
    {// 1
        "Umwälzpumpe",
        {0x31, 0x00, 0xFA, 0xC0, 0xF7, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xF7,   DC,   DC},
        [](auto const& data) -> std::string {
            float status = float((float((int((data[6]) + ((data[5])))))));
            id(Umwaelzpumpe).publish_state(status);
            return Utils::str_format("Umwälzpumpe: %f", status);
        }
    },
    {// 2
        "Betriebsmodus",
        {0x31, 0x00, 0xFA, 0x01, 0x12, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x12,   DC,   DC},
        [](auto const& data) -> std::string {
            id(Betriebsmodus).publish_state(data[5]);
            const std::string mode = Utils::setSelectOption(id(select_betriebsmodus), map_betriebsmodus, data[5]);
            return Utils::str_format("Betriebsmodus: %s", mode.c_str());
        }
    },
    {// 3
        "Betriebsart",
        {0x31, 0x00, 0xFA, 0xC0, 0xF6, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xF6,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5])))))));
            id(Betriebsart).publish_state(temperature);
            return Utils::str_format("Betriebsart: %f", temperature);
        }
    },
    {// 4
        "VL Soll",
        {0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00},
        {0xD2,   DC, 0x02,   DC,   DC,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(vl_soll).publish_state(temperature);
            return Utils::str_format("VL Soll: %f", temperature);
        }
    },
    {// 5
        "BPV",
        {0x31, 0x00, 0xFA, 0xC0, 0xFB, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFB,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(BPV).publish_state(temperature);
            return Utils::str_format("BPV: %f", temperature);
        }
    },
    {// 6
        "Durchfluss",
        {0x31, 0x00, 0xFA, 0x01, 0xDA, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0xDA,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(durchfluss).publish_state(temperature);
            return Utils::str_format("Volumenstrom: %f", temperature);
        }
    },
    {// 7
        "Laufzeit Pumpe",
        {0x31, 0x00, 0xFA, 0x06, 0xA4, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0xA4,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_pump).publish_state(temperature);
            return Utils::str_format("Laufzeit Pump: %f", temperature);
        }
    },
    {// 8
        "Vorlauftemperatur (TVBH)",
        {0x31, 0x00, 0xFA, 0xC1, 0x02, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC1, 0x02,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TVBH).publish_state(temperature);
            return Utils::str_format("Vorlauftemperatur: %f", temperature);
        }
    },
    {// 9
        "Vorlauftemperatur",
        {0x31, 0x00, 0xFA, 0xC0, 0xFC, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFC,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(TV).publish_state(temperature);
            return Utils::str_format("Vorlauftemperatur: %f", temperature);
        }
    },
    {// 10
        "Rücklauftemperatur Heizung",
        {0x31, 0x00, 0xFA, 0xC1, 0x00, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC1, 0x00,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(ruecklauf).publish_state(temperature);
            return Utils::str_format("Rücklauftemperatur: %f", temperature);
        }
    },
    {// 11
        "Warmwassertemperatur",
        {0x31, 0x00, 0xFA, 0x00, 0x0E, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x00, 0x0E,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(temperature_water).publish_state(temperature);
            return Utils::str_format("Warmwassertemperatur: %f", temperature);
        }
    },
    {// 12
        "Warmwasser soll",
        {0x31, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00},
        {0xD2, 0x00, 0x13,   DC,   DC, 0x00, 0x00},
        [](auto const& data) -> std::string {
            uint16_t temperature_raw = (data[3] <<  8) | data[4]; // Convert to int16be
            float temperature = static_cast<float>(temperature_raw) / 10.0;
            id(t_ww_soll).publish_state(temperature);
            return Utils::str_format("Warmwasser soll: %f", temperature);
        }
    },
    {// 13
        "Raumsoll 1",
        {0x31, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00},
        {0xD2, 0x00, 0x05, 0x00,   DC, 0x00,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(raumsoll1).publish_state(temperature);
            return Utils::str_format("Raumsoll 1: %f", temperature);
        }
    },
    {// 14
        "Laufzeit Kompressor",
        {0x31, 0x00, 0xFA, 0x06, 0xA5, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0xA5,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(rt_compressor).publish_state(temperature);
            return Utils::str_format("Laufzeit Kompressor: %f", temperature);
        }
    },
    {// 15
        "Min Vl Soll",
        {0x31, 0x00, 0xFA, 0x01, 0x2B, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x2B,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float(float((int((data[6]) + ((data[5]) << 8))))/10);
            id(min_vl_soll).publish_state(temperature);
            return Utils::str_format("Min Vl Soll: %f", temperature);
        }
    },
    {// 16
        "Max Vl Soll",
        {0x31, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00},
        {  DC,   DC, 0x28,   DC,   DC,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/10));
            id(max_vl_soll).publish_state(temperature);
            return Utils::str_format("Max Vl Soll: %f", temperature);
        }
    },
    {// 17
        "HK Funktion",
        {0x31, 0x00, 0xFA, 0x01, 0x41, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x41,   DC,   DC},
        [](auto const& data) -> std::string {
            id(hk_funktion).publish_state(data[6]);
            const std::string mode = Utils::setSelectOption(id(select_hk), map_hk, data[6]);
            return Utils::str_format("HK Funktion: %s", mode.c_str());
        }
    },
    {// 18
        "Temperatur Vorlauf Tag",
        {0x31, 0x00, 0xFA, 0x01, 0x29, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x29,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(t_vorlauf_tag).publish_state(temperature);
            return Utils::str_format("Temperatur Vorlauf Tag: %f", temperature);
        }
    },
    {// 19
        "Fehlercode",
        {0x31, 0x00, 0xFA, 0x13, 0x88, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x13, 0x88,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Fehlercode).publish_state(temperature);
            return Utils::str_format("Fehlercode: %f", temperature);
        }
    },
    {// 20
        "Erzeugte Energie gesamt",
        {0x31, 0x00, 0xFA, 0x09, 0x30, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x09, 0x30,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Erzeugte_Energie_Gesamt).publish_state(temperature);
            return Utils::str_format("Erzeugte Energie gesamt: %f", temperature);
        }
    },
    {// 21
        "Wasserdruck",
        {0x31, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00},
        {0xD2,   DC, 0x1C,   DC,   DC,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[4]) + ((data[3]) << 8))))/1000));
            id(Wasserdruck).publish_state(temperature);
            return Utils::str_format("Wasserdruck: %f", temperature);
        }
    },
    {// 22
        "EHS für CH",
        {0x31, 0x00, 0xFA, 0x09, 0x20, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x09, 0x20,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(Qchhp).publish_state(temperature);
            return Utils::str_format("EHS fuer CH: %f", temperature);
        }
    },
    {// 23
        "Außentemperatur",
        {0x31, 0x00, 0xFA, 0xC0, 0xFF, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0xC0, 0xFF,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float(((int((data[6]) + ((data[5]) << 8))) ^ 0x8000) - 0x8000)/10;
            id(temperature_outside).publish_state(temperature); 
            return Utils::str_format("Außentemperatur: %f", temperature);
        }
    },
    {// 24
        "Heizkurve",
        {0x31, 0x00, 0xFA, 0x01, 0x0E, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x01, 0x0E,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/100));
            id(heizkurve).publish_state(temperature);
            return Utils::str_format("Heizkurve: %f", temperature);
        }
    },
    {// 25
        "Spreizung MOD HZ",
        {0x31, 0x00, 0xFA, 0x06, 0x83, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x83,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_hz).publish_state(temperature);
            return Utils::str_format("Spreizung MOD HZ: %f", temperature);
        }
    },
    {// 26
        "Spreizung MOD WW",
        {0x31, 0x00, 0xFA, 0x06, 0x84, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x84,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[6]) + ((data[5]) << 8))))/10));
            id(spreizung_mod_ww).publish_state(temperature);
            return Utils::str_format("Spreizung MOD WW: %f", temperature);
        }
    },
    {// 27
        "SGModus",
        {0x31, 0x00, 0xFA, 0x06, 0x94, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x94,   DC,   DC},
        [](auto const& data) -> std::string {
            id(SGModus).publish_state(data[6]);
            const std::string mode = Utils::setSelectOption(id(select_sg_mode), map_sg_mode, data[6]);
            return Utils::str_format("SGModus: %s", mode.c_str());
        }
    },
    {// 28
        "Smart Grid",
        {0x31, 0x00, 0xFA, 0x06, 0x93, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x93,   DC,   DC},
        [](auto const& data) -> std::string {
            id(Smartgrid).publish_state(data[6]);
            const std::string mode = Utils::setSelectOption(id(select_smartgrid), map_sg, data[6]);
            return Utils::str_format("Smart Grid: %s", mode.c_str());
        }
    },
    {// 29
        "Status Kompressor",
        {0xA1, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00},
        0x500,
        {  DC,   DC, 0x61,   DC,   DC,   DC,   DC},
        [](auto const& data) -> std::string {
            float temperature = float((float((int((data[3]) + ((data[4]) << 8))))));
            id(status_kompressor).publish_state(temperature);
            return Utils::str_format("Status Kompressor: %f", temperature);
        }
    },
    {// 30
        "Status Mischer DHW",
        {0x31, 0x00, 0xFA, 0x06, 0x9B, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x06, 0x9B,   DC,   DC},
        [](auto const& data) -> std::string {
            float status = float((float((int((data[6]) + ((data[5]) << 8))))));
            id(dhw_mischer_postion).publish_state(status);
            return Utils::str_format("Status Mischer DHW: %f", status);
        }
    },
    {// 31
        "Heizstabrelais",
        {0x31, 0x00, 0xFA, 0x0A, 0x20, 0x00, 0x00},
        {0xD2, 0x00, 0xFA, 0x0A, 0x20, DC, DC},
        [](auto const& data) -> std::string {
            const bool r1 = (data[5] == 0) && (data[6] & 0x40) == 0x40;
            const bool r2 = (data[5] == 0) && (data[6] & 0x80) == 0x80;
            const bool r3 = (data[5] == 1) && (data[6] == 0);
            id(status_heizstab).publish_state((0x4 * r1) | (0x2 * r2) | r3);
            return Utils::str_format("Heizstabrelais: %d|%d|%d status: %d", r1, r2, r3, status_heizstab);
        }
    },  // Status Heizstäbe
    /*{// 32
        "zzz",
        {0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00},
        {0x31, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00},
        [](auto const& data) -> std::string {
            return Utils::str_format("unbekannt!!!", "");
        }
    } // Warmwasser Temperatur  °C et dec value - ok /10
    */
};

TRequests data_requests(std::move(entity_config));