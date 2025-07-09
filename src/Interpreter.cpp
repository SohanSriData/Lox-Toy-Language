#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <iostream>
using namespace std;

#include "Expr.h"
#include "Stmt.h"
#include "Environment.h"
#include "RuntimeError.h"
#include "LoxCallable.h"
#include "LoxFunction.h" // Use header, not .cpp

// Helper: Convert literal variant to Object
inline Object literalToObject(const std::variant<std::monostate, double, std::string, bool>& lit) {
    return std::visit([](auto&& val) -> Object { return val; }, lit);
}

// ---- Interpreter Method Implementations ----

Interpreter::Interpreter() {
    globals = make_shared<Environment>();
    environment = globals;
}

shared_ptr<Environment> Interpreter::getGlobals() const { return globals; }
shared_ptr<Environment> Interpreter::getEnvironment() const { return environment; }

void Interpreter::executeBlock(vector<shared_ptr<Stmt>> statements, shared_ptr<Environment> newEnv) {
    auto previous = environment;
    environment = newEnv;
    try {
        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (RuntimeError& error) {
        cerr << "Runtime error: " << error.what() << endl;
    }
    environment = previous;
}

Object Interpreter::visitLiteralExpr(Literal* expr) {
    return literalToObject(expr->value.literal);
}

Object Interpreter::visitLogicalExpr(Logical* expr) {
    Object left = evaluate(expr->left);
    if (expr->operatorToken.type == TokenType::OR) {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }
    return evaluate(expr->right);
}

Object Interpreter::visitGroupingExpr(Grouping* expr) {
    return evaluate(expr->expression);
}

Object Interpreter::visitUnaryExpr(Unary* expr) {
    Object right = evaluate(expr->right);
    switch (expr->operatorToken.type) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            checkNumberOperand(expr->operatorToken, right);
            return -get<double>(right);
        default:
            return std::monostate{};
    }
}

Object Interpreter::visitBinaryExpr(Binary* expr) {
    Object left = evaluate(expr->left);
    Object right = evaluate(expr->right);

    switch (expr->operatorToken.type) {
        case TokenType::MINUS:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) - get<double>(right);
        case TokenType::SLASH:
            checkNumberOperands(expr->operatorToken, left, right);
            if (get<double>(right) == 0) throw RuntimeError("Division by zero.");
            return get<double>(left) / get<double>(right);
        case TokenType::STAR:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) * get<double>(right);
        case TokenType::PLUS:
            if (holds_alternative<double>(left) && holds_alternative<double>(right))
                return get<double>(left) + get<double>(right);
            if (holds_alternative<string>(left) && holds_alternative<string>(right))
                return get<string>(left) + get<string>(right);
            throw RuntimeError("Operands must be two numbers or two strings.");
        case TokenType::GREATER:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) > get<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) >= get<double>(right);
        case TokenType::LESS:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) < get<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr->operatorToken, left, right);
            return get<double>(left) <= get<double>(right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        default:
            cerr << "Unknown operator: " << expr->operatorToken.lexeme << endl;
            return std::monostate{};
    }
}

Object Interpreter::visitCallExpr(Call* expr) {
    Object callee = evaluate(expr->callee);
    vector<Object> arguments;
    for (const auto& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    if (!holds_alternative<shared_ptr<LoxCallable>>(callee)) {
        throw RuntimeError("Can only call functions and classes.");
    }
    auto function = get<shared_ptr<LoxCallable>>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError("Expected " + to_string(function->arity()) + " arguments but got " + to_string(arguments.size()) + ".");
    }
    return function->call(this, arguments);
}

Object Interpreter::visitVariableExpr(Variable* expr) {
    return environment->get(expr->name.lexeme);
}

Object Interpreter::visitAssignExpr(Assign* expr) {
    Object value = evaluate(expr->value);
    environment->assign(expr->name.lexeme, value);
    return value;
}

void Interpreter::interpret(vector<shared_ptr<Stmt>> statements) {
    try {
        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (RuntimeError& error) {
        cerr << "Runtime error: " << error.what() << endl;
    }
}

Object Interpreter::visitBlockStmt(Block* stmt) {
    executeBlock(stmt->statements, make_shared<Environment>(environment));
    return std::monostate{};
}

Object Interpreter::visitExpressionStmt(Expression* stmt) {
    evaluate(stmt->expression);
    return std::monostate{};
}

Object Interpreter::visitFunctionStmt(Function* stmt) {
    auto function = make_shared<LoxFunction>(stmt);
    environment->define(stmt->name.lexeme, function);
    return std::monostate{};
}

Object Interpreter::visitIfStmt(If* stmt) {
    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr) {
        execute(stmt->elseBranch);
    }
    return std::monostate{};
}

Object Interpreter::visitPrintStmt(Print* stmt) {
    Object value = evaluate(stmt->expression);
    cout << stringify(value) << endl;
    return std::monostate{};
}

Object Interpreter::visitVarStmt(Var* stmt) {
    Object value = std::monostate{};
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }
    environment->define(stmt->name.lexeme, value);
    return std::monostate{};
}

Object Interpreter::visitWhileStmt(While* stmt) {
    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
    return std::monostate{};
}

// ---- Private Helper Methods ----

Object Interpreter::evaluate(shared_ptr<Expr> expr) {
    return expr->accept(this);
}

void Interpreter::execute(shared_ptr<Stmt> stmt) {
    stmt->accept(this);
}

bool Interpreter::isTruthy(Object object) {
    if (holds_alternative<std::monostate>(object)) return false;
    if (holds_alternative<bool>(object)) return get<bool>(object);
    if (holds_alternative<double>(object)) return get<double>(object) != 0;
    if (holds_alternative<string>(object)) return !get<string>(object).empty();
    return false;
}

bool Interpreter::isEqual(Object left, Object right) {
    if (holds_alternative<std::monostate>(left) && holds_alternative<std::monostate>(right)) return true;
    if (holds_alternative<double>(left) && holds_alternative<double>(right))
        return get<double>(left) == get<double>(right);
    if (holds_alternative<string>(left) && holds_alternative<string>(right))
        return get<string>(left) == get<string>(right);
    if (holds_alternative<bool>(left) && holds_alternative<bool>(right))
        return get<bool>(left) == get<bool>(right);
    return false;
}

void Interpreter::checkNumberOperand(Token operatorToken, Object operand) {
    if (!holds_alternative<double>(operand)) {
        throw RuntimeError("Operand must be a number.");
    }
}

void Interpreter::checkNumberOperands(Token operatorToken, Object left, Object right) {
    if (!holds_alternative<double>(left) || !holds_alternative<double>(right)) {
        throw RuntimeError("Operands must be numbers.");
    }
}

string Interpreter::stringify(Object object) {
    if (holds_alternative<std::monostate>(object)) return "nil";
    if (holds_alternative<double>(object)) return to_string(get<double>(object));
    if (holds_alternative<string>(object)) return get<string>(object);
    if (holds_alternative<bool>(object)) return get<bool>(object) ? "true" : "false";
    return "unknown";
}
