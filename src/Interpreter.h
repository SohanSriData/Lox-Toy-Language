// #pragma once
// #include <memory>
// #include <vector>
// #include <string>
// #include <variant>
// #include "Expr.h"
// #include "Stmt.h"
// #include "Environment.h"
// #include "RuntimeError.h"
// #include "LoxCallable.h"

// class Interpreter : public Expr::Visitor, public Stmt::Visitor {
// public:
//     Interpreter();

//     void interpret(std::vector<std::shared_ptr<Stmt>> statements);
//     shared_ptr<Environment> getGlobals() const;
//     shared_ptr<Environment> getEnvironment() const;

//     void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> newEnv);

//     // Expr visitors
//     Object visitLiteralExpr(Literal* expr) override;
//     Object visitLogicalExpr(Logical* expr) override;
//     Object visitGroupingExpr(Grouping* expr) override;
//     Object visitUnaryExpr(Unary* expr) override;
//     Object visitBinaryExpr(Binary* expr) override;
//     Object visitCallExpr(Call* expr) override;
//     Object visitVariableExpr(Variable* expr) override;
//     Object visitAssignExpr(Assign* expr) override;

//     // Stmt visitors
//     Object visitBlockStmt(Block* stmt) override;
//     Object visitExpressionStmt(Expression* stmt) override;
//     //Object visitFunctionStmt(Function* stmt) override;
//     Object visitIfStmt(If* stmt) override;
//     Object visitPrintStmt(Print* stmt) override;
//     Object visitVarStmt(Var* stmt) override;
//     Object visitWhileStmt(While* stmt) override;

// private:
//     std::shared_ptr<Environment> environment;
//     std::shared_ptr<Environment> globals;

//     Object evaluate(std::shared_ptr<Expr> expr);
//     void execute(std::shared_ptr<Stmt> stmt);
    

//     bool isTruthy(Object object);
//     bool isEqual(Object left, Object right);
//     void checkNumberOperand(Token operatorToken, Object operand);
//     void checkNumberOperands(Token operatorToken, Object left, Object right);
//     std::string stringify(Object object);
// };