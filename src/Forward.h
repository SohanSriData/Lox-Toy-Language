#pragma once
#include <variant>
#include <string>
#include <memory>
class LoxCallable;
class Expr;
class Stmt;

using std::shared_ptr;
using std::string;

using Object = std::variant<
    std::monostate,
    double,
    string,
    bool,
    shared_ptr<LoxCallable>
>;
