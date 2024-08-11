#pragma once

template<typename KeyType, typename ValueType>
class BidiMap {
public:
    using Iterator = typename std::map<KeyType, ValueType>::const_iterator;

    BidiMap(std::initializer_list<std::pair<const KeyType, ValueType>> init_list)
    : key_to_value(init_list) {
        for (const auto& pair : init_list) {
            value_to_key[pair.second] = pair.first;
        }
    }

    Iterator findByKey(const KeyType& key) const {
        return key_to_value.find(key);
    }

    Iterator findByValue(const ValueType& value) const {
        auto it = value_to_key.find(value);
        if (it != value_to_key.end()) {
            return key_to_value.find(it->second);
        }
        return key_to_value.end();
    }

    KeyType getKey(const ValueType& value) const {
        auto it = findByValue(value);
        if (it != key_to_value.end()) {
            return it->first;
        }
        throw std::runtime_error("Value not found");
    }

    ValueType getValue(const KeyType& key) const {
        auto it = findByKey(key);
        if (it != key_to_value.end()) {
            return it->second;
        }
        throw std::runtime_error("Key not found");
    }

    Iterator end() const {
        return key_to_value.end();
    }

private:
    std::map<KeyType, ValueType> key_to_value;
    std::map<ValueType, KeyType> value_to_key;
};
