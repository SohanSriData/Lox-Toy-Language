// LoxFunction.cpp

#include "LoxFunction.h"
#include "Return.cpp"
#include <iostream>

LoxFunction::LoxFunction(Function* declaration)
    : declaration(declaration) {
}

Object LoxFunction::call(Interpreter* interpreter,
                         vector<Object> arguments) {
    // Create a new environment for the function call
    shared_ptr<Environment> environment = make_shared<Environment>(interpreter->getGlobals());

    for (size_t i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].lexeme, arguments[i]);
    }
    try{
    interpreter->executeBlock(declaration->body, environment);
    } catch (ReturnClass& returnValue) {
        // If a return statement was encountered, return its value
        return returnValue.getValue();
    } 
    return std::monostate();
}

size_t LoxFunction::arity() const {
    return declaration->params.size();
}

std::string LoxFunction::toString() {
    return "<function " + declaration->name.lexeme + ">";
}
