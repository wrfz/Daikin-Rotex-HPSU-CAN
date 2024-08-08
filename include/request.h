#pragma once

#include "utils.h"

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
        for (uint32_t index = 0; index < data.size(); ++index) {
            if (m_expected_reponse[index] != DC && data[index] != m_expected_reponse[index]) {
                return false;
            }
        }
        return true;
    }

    bool handle(uint32_t can_id, std::vector<uint8_t> const& data) const {
        if (m_lambda_used && can_id == m_response_can_id && startsWith(data)) {
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
    , request_index(0u)
    {
    }

    uint32_t size() const {
        return m_requests.size();
    }

    TRequest const& get(uint32_t index) const {
        return m_requests[index];
    }

    bool handle(esphome::esp32_can::ESP32Can* can_bus, uint32_t can_id, std::vector<uint8_t> const& data, uint32_t timestamp) const {
        ESP_LOG_FILTER("handle", "can_id<%s> data<%s>", to_hex(can_id).c_str(), to_hex(data).c_str());
        for (auto const& request : m_requests) {
            if (request.handle(can_id, data)) {
                return true;
            }
        }
        return false;
    }

    void sendRequest(esphome::esp32_can::ESP32Can* can_bus, uint32_t message_index = 0xFFFFFFFF)
    {
        if (message_index != 0xFFFFFFFF) {
            request_index = message_index;
        }
        if (request_index >= size()) {
            request_index = 0;
        }

        TRequest const& request = get(request_index);
        const uint32_t can_id = 0x680;
        const bool use_extended_id = false;
        std::array<uint8_t, 7> const& data = request.data();

        can_bus->send_data(can_id, use_extended_id, { data.begin(), data.end() });
        ESP_LOG_FILTER("main", "request(%d, %s, %s)", request_index++, request.getName().c_str(), to_hex(request.data()).c_str());
    }
private:
    std::vector<TRequest> m_requests;
    uint32_t request_index;
};



