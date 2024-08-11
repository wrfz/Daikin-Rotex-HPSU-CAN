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
        std::function<DataType(std::vector<uint8_t> const&)> lambda)
    : m_name(name)
    , m_data(data)
    , m_response_can_id(response_can_id)
    , m_expected_reponse(expected_reponse)
    , m_lambda(lambda)
    , m_last_update(0u)
    , m_last_request(0u)
    {
    }

    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        std::array<uint16_t, 7> const& expected_reponse,
        std::function<DataType(std::vector<uint8_t> const&)> lambda)
    : TRequest(name, data, 0x180, expected_reponse, lambda)
    {
    }

    void init(uint32_t request_index) {
        m_request_index = request_index;
    }

    uint32_t getLastUpdate() const {
        return m_last_update;
    }

    bool isMatch(uint32_t can_id, std::vector<uint8_t> const& responseData) const {
        if (can_id == m_response_can_id) {
            for (uint32_t index = 0; index < responseData.size(); ++index) {
                if (m_expected_reponse[index] != DC && responseData[index] != m_expected_reponse[index]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool handle(uint32_t can_id, std::vector<uint8_t> const& responseData, uint32_t timestamp) {
        if (isMatch(can_id, responseData)) {
            DataType variant = m_lambda(responseData);
            std::string value;
            if (std::holds_alternative<uint32_t>(variant)) {
                value = std::to_string(std::get<uint32_t>(variant));
            } else if (std::holds_alternative<float>(variant)) {
                value = std::to_string(std::get<float>(variant));
            } else if (std::holds_alternative<std::string>(variant)) {
                value = std::get<std::string>(variant);
            } else {
                value = "Unsupported value type!";
            }
            Utils::log("handled", "%s<%s> can_id<%s> data<%s>",
                m_name.c_str(), value.c_str(), Utils::to_hex(can_id).c_str(), Utils::to_hex(responseData).c_str());

            m_last_update = timestamp;
            return true;
        }
        return false;
    }

    void request(esphome::esp32_can::ESP32Can* pCanBus) {
        const uint32_t can_id = 0x680;
        const bool use_extended_id = false;

        pCanBus->send_data(can_id, use_extended_id, { m_data.begin(), m_data.end() });
        Utils::log("request", "can_id<%d> name<%s> data<%s>", m_request_index, m_name.c_str(), Utils::to_hex(m_data).c_str());

        m_last_request = millis();
    }

    bool inProgress() const {
        return m_last_request > m_last_update && ((millis() - m_last_request) < 3*1000);
    }

private:
    uint32_t m_request_index;
    std::string m_name;
    std::array<uint8_t, 7> m_data;
    uint32_t m_response_can_id;
    std::array<uint16_t, 7> m_expected_reponse;
    std::function<DataType(std::vector<uint8_t> const&)> m_lambda;
    uint32_t m_last_update;
    uint32_t m_last_request;
};

class TRequests {
public:
    TRequests(std::vector<TRequest> const& requests)
    : m_requests(requests)
    {
        uint32_t index = 0;
        for (TRequest& request : m_requests) {
            request.init(index++);
        }
    }

    uint32_t size() const {
        return m_requests.size();
    }

    TRequest const& get(uint32_t index) const {
        return m_requests[index];
    }

    void handle(esphome::esp32_can::ESP32Can* pCanBus, uint32_t can_id, std::vector<uint8_t> const& responseData, uint32_t timestamp) {
        TRequest* pRequest = findRequest(can_id, responseData);
        if (pRequest != nullptr) {
            pRequest->handle(can_id, responseData, timestamp);
        } else {
            Utils::log("unhandled", "can_id<%s> data<%s>", Utils::to_hex(can_id).c_str(), Utils::to_hex(responseData).c_str());
        }
    }

    bool sendNextPendingRequest(esphome::esp32_can::ESP32Can* pCanBus) {
        TRequest* pRequest = getNextRequestToSend();
        if (pRequest != nullptr) {
            pRequest->request(pCanBus);
            return true;
        }
        return false;
    }

    void sendRequest(esphome::esp32_can::ESP32Can* pCanBus, uint32_t message_index) {
        if (message_index >= 0 && message_index < m_requests.size()) {
            TRequest& request = m_requests[message_index];
            request.request(pCanBus);
        }
    }

private:
    TRequest* findRequest(uint32_t can_id, std::vector<uint8_t> const& responseData) {
        for (auto& request : m_requests) {
            if (request.isMatch(can_id, responseData)) {
                return &request;
            }
        }
        return nullptr;
    }

    TRequest* getNextRequestToSend() {
        const uint32_t timestamp = millis();
        const uint32_t interval = static_cast<uint32_t>(id(update_interval).state) * 1000;

        for (auto& request : m_requests) {
            if ((timestamp > (request.getLastUpdate() + interval)) && !request.inProgress()) {
                return &request;
            }
        }
        return nullptr;
    }

    std::vector<TRequest> m_requests;
};



