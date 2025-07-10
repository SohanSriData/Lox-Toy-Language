#pragma once

#include <stdexcept>
#include "Forward.h"

class ReturnClass : public std::exception {
private:
    Object value;

public:
    explicit ReturnClass(const Object& value)
        : value(value) {}

    const Object& getValue() const {
        return value;
    }

    const char* what() const noexcept override {
        return "Return exception";
    }
};
