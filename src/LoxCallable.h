#pragma once
#include <string>
#include <variant>
#include <vector>
#include "Interpreter.h"
#include "Forward.h"
using namespace std;
using Object = std::variant<std::monostate, double, std::string, bool, shared_ptr<LoxCallable>>;


class LoxCallable {
public:
    virtual size_t arity() const = 0; // Returns the number of parameters the callable expects
    virtual Object call(Interpreter* interpreter, vector<Object> arguments) = 0;
    LoxCallable() = default;
    virtual ~LoxCallable();
};