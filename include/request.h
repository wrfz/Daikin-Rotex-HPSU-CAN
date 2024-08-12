#pragma once

#include "utils.h"

const uint16_t DC = 0xFFFF; // Don't care

class TRequest
{
    using TGetLambda = std::function<DataType(std::vector<uint8_t> const&)>;
    using TSetLambda = std::function<std::vector<uint8_t>(float const&)>;
public:
    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        uint32_t response_can_id,
        std::array<uint16_t, 7> const& expected_reponse,
        TGetLambda lambda,
        TSetLambda setLambda)
    : m_name(name)
    , m_data(data)
    , m_response_can_id(response_can_id)
    , m_expected_reponse(expected_reponse)
    , m_lambda(lambda)
    , m_set_lambda(setLambda)
    , m_last_update(0u)
    , m_last_request(0u)
    {
    }

    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        uint32_t response_can_id,
        std::array<uint16_t, 7> const& expected_reponse,
        TGetLambda lambda)
    : TRequest(name, data, response_can_id, expected_reponse, lambda, [](float) -> std::vector<uint8_t> { return {}; })
    {
    }

    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        std::array<uint16_t, 7> const& expected_reponse,
        TGetLambda lambda,
        TSetLambda setLambda)
    : TRequest(name, data, 0x180, expected_reponse, lambda, setLambda)
    {
    }

    TRequest(std::string const& name,
        std::array<uint8_t, 7> const& data,
        std::array<uint16_t, 7> const& expected_reponse,
        TGetLambda lambda)
    : TRequest(name, data, 0x180, expected_reponse, lambda)
    {
    }

    std::string const& getName() const {
        return m_name;
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
            Utils::log("request.h", "handled: %s<%s> can_id<%s> data<%s>",
                m_name.c_str(), value.c_str(), Utils::to_hex(can_id).c_str(), Utils::to_hex(responseData).c_str());

            m_last_update = timestamp;
            return true;
        }
        return false;
    }

    void sendGet(esphome::esp32_can::ESP32Can* pCanBus) {
        const uint32_t can_id = 0x680;
        const bool use_extended_id = false;

        pCanBus->send_data(can_id, use_extended_id, { m_data.begin(), m_data.end() });
        Utils::log("request.h", "sendGet: name<%s> can_id<%s> data<%s>",
            m_name.c_str(), Utils::to_hex(can_id).c_str(), Utils::to_hex(m_data).c_str());

        m_last_request = millis();
    }

    void sendSet(esphome::esp32_can::ESP32Can* pCanBus, float value) {
        const uint32_t can_id = 0x680;
        const bool use_extended_id = false;

        auto data = m_set_lambda(value);

        pCanBus->send_data(can_id, use_extended_id, { data.begin(), data.end() });
        Utils::log("request.h", "sendSet: name<%s> value<%f> can_id<%s> data<%s>",
            m_name.c_str(), value, Utils::to_hex(can_id).c_str(), Utils::to_hex(data).c_str());

        sendGet(pCanBus);
    }

    bool inProgress() const {
        return m_last_request > m_last_update && ((millis() - m_last_request) < 3*1000);
    }

private:
    std::string m_name;
    std::array<uint8_t, 7> m_data;
    uint32_t m_response_can_id;
    std::array<uint16_t, 7> m_expected_reponse;
    TGetLambda m_lambda;
    std::function<std::vector<uint8_t>(float const&)> m_set_lambda;
    uint32_t m_last_update;
    uint32_t m_last_request;
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

    void handle(esphome::esp32_can::ESP32Can* pCanBus, uint32_t can_id, std::vector<uint8_t> const& responseData, uint32_t timestamp) {
        bool bHandled = false;
        for (auto& request : m_requests) {
            if (request.isMatch(can_id, responseData)) {
                request.handle(can_id, responseData, timestamp);
                bHandled = true;
            }
        }
        if (!bHandled) {
            Utils::log("request.h", "unhandled: can_id<%s> data<%s>", Utils::to_hex(can_id).c_str(), Utils::to_hex(responseData).c_str());
        }
    }

    bool sendNextPendingGet(esphome::esp32_can::ESP32Can* pCanBus) {
        TRequest* pRequest = getNextRequestToSend();
        if (pRequest != nullptr) {
            pRequest->sendGet(pCanBus);
            return true;
        }
        return false;
    }

    void sendGet(esphome::esp32_can::ESP32Can* pCanBus, std::string const& request_name) {
        const auto it = std::find_if(m_requests.begin(), m_requests.end(),
            [& request_name](auto& request) { return request.getName() == request_name; });

        if (it != m_requests.end()) {
            it->sendGet(pCanBus);
        } else {
            Utils::log("request.h", "sendGet(%s) -> Unknown request!", request_name.c_str());
        }
    }

    void sendSet(esphome::esp32_can::ESP32Can* pCanBus, std::string const& request_name, float value) {
        const auto it = std::find_if(m_requests.begin(), m_requests.end(),
            [& request_name](auto& request) { return request.getName() == request_name; });

        if (it != m_requests.end()) {
            it->sendSet(pCanBus, value);
        } else {
            Utils::log("request.h", "sendSet(%s) -> Unknown request!", request_name.c_str());
        }
    }

private:
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



