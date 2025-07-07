#pragma once
#include <stdexcept>
#include <string>

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& message)
        : std::runtime_error(message) {}
};