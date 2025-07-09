// LoxFunction.cpp

#include "LoxFunction.h"

LoxFunction::LoxFunction(Function* declaration)
    : declaration(declaration) {
}

Object LoxFunction::call(Interpreter* interpreter,
                         std::vector<Object> arguments) {
    // Create a new environment for the function call
    Environment environment(interpreter->getGlobals());

    for (size_t i = 0; i < declaration->params.size(); i++) {
        environment.define(declaration->params[i].lexeme, arguments[i]);
    }

    interpreter->executeBlock(
        declaration->body,
        std::make_shared<Environment>(environment));

    return std::monostate();
}

size_t LoxFunction::arity() const {
    return declaration->params.size();
}

std::string LoxFunction::toString() {
    return "<function " + declaration->name.lexeme + ">";
}
