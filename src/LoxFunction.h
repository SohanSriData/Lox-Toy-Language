// LoxFunction.h
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "LoxCallable.h"
#include "Forward.h"
#include "Environment.h"
#include "Stmt.h"          // For Function
#include "Interpreter.h"

class LoxFunction : public LoxCallable {
private:
    Function* declaration;

public:
    LoxFunction(Function* declaration);

    Object call(Interpreter* interpreter,
                std::vector<Object> arguments) override;

    size_t arity() const override;

    std::string toString();
};
