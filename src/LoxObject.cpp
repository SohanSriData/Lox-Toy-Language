// #pragma once
// using namespace std;
// #include <variant>
// #include <string>
// #include <memory>
// #include <vector>
// #include "LoxCallable.h"

// struct LoxObject {
//     virtual ~LoxObject() = default;
//     virtual string toString() const = 0; // Convert the object to a string representation
//     virtual bool isTruthy() const { return true; } // Default truthiness check
// };
// using LoxObjectPtr = shared_ptr<LoxObject>;



// struct LoxNumber : public LoxObject{
//     double value;
//     LoxNumber(double value) : value(value) {}
//     string toString() const override {
//         return to_string(value);
//     }
//     bool isTruthy() const override{
//         return value != 0;
//     }
// };

// struct LoxString : public LoxObject {
//     string value;
//     LoxString(const string& value) :value(value){}
//     string toString() const override {
//         return value;
//     }
//     bool isTruthy() const override {
//         return !value.empty();
//     }
// };

// struct LoxBool : public LoxObject {
//     bool value;
//     LoxBool(bool value) : value(value) {}
//     string toString() const override {
//         return value ? "true" : "false";
//     }
//     bool isTruthy() const override {
//         return value;
//     }
// };

// struct LoxNil : public LoxObject {
//     string toString() const override {
//         return "nil";
//     }
//     bool isTruthy() const override {
//         return false; // nil is always false in truthiness checks
//     }
// };


