#include "Expr.h"
#pragma once
#include <memory>
#include "Token.h"
using namespace std;

using Object = std::variant<std::monostate, double, std::string, bool>;

class Stmt {
public:
    virtual ~Stmt() = default;
class Visitor {
public:
   virtual Object visitBlockStmt(class Block* Stmt) = 0;
   virtual Object visitExpressionStmt(class Expression* Stmt) = 0;
   virtual Object visitIfStmt(class If* Stmt) = 0;
   virtual Object visitPrintStmt(class Print* Stmt) = 0;
   virtual Object visitVarStmt(class Var* Stmt) = 0;
   virtual Object visitWhileStmt(class While* Stmt) = 0;
};

virtual Object accept(class Visitor* visitor) =0; 
};

class Block : public Stmt {
public:
    Block(vector<shared_ptr<Stmt>> statements) : statements(statements) {}
    vector<shared_ptr<Stmt>> statements;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitBlockStmt(this);
}

};

class Expression : public Stmt {
public:
    Expression(shared_ptr<Expr> expression) : expression(expression) {}
    shared_ptr<Expr> expression;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitExpressionStmt(this);
}

};

class If : public Stmt {
public:
    If(shared_ptr<Expr> condition, shared_ptr<Stmt> thenBranch, shared_ptr<Stmt> elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
    shared_ptr<Expr> condition;
    shared_ptr<Stmt> thenBranch;
    shared_ptr<Stmt> elseBranch;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitIfStmt(this);
}

};

class Print : public Stmt {
public:
    Print(shared_ptr<Expr> expression) : expression(expression) {}
    shared_ptr<Expr> expression;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitPrintStmt(this);
}

};

class Var : public Stmt {
public:
    Var(Token name, shared_ptr<Expr> initializer) : name(name), initializer(initializer) {}
    Token name;
    shared_ptr<Expr> initializer;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitVarStmt(this);
}

};

class While : public Stmt {
public:
    While(shared_ptr<Expr> condition, shared_ptr<Stmt> body) : condition(condition), body(body) {}
    shared_ptr<Expr> condition;
    shared_ptr<Stmt> body;
	Object accept (Stmt::Visitor* visitor) override {
    return visitor->visitWhileStmt(this);
}

};


