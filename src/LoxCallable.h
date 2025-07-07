// #pragma once
// #include <string>
// #include <variant>
// #include <vector>
// #include "Interpreter.h"
// using namespace std;
// using Object = variant<std::monostate, double, std::string, bool>;


// class LoxCallable {
// public:
//     virtual size_t arity() const; // Returns the number of parameters the callable expects
//     virtual Object call(Interpreter* interpreter, vector<Object> arguments) = 0;
//     virtual ~LoxCallable() = default;
// };