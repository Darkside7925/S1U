#pragma once

#include <vector>
#include <string>
#include <memory>

namespace s1u {

class QuantumProtocol {
public:
    QuantumProtocol() = default;
    ~QuantumProtocol() = default;

    bool initialize() {
        return true;
    }

    void shutdown() {}

    bool send_message(const std::string& message) {
        return true;
    }

    std::string receive_message() {
        return "";
    }

private:
};

} // namespace s1u


