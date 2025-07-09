#pragma once
#include <memory>
#include "Token.h"
#include "Forward.h"
using namespace std;

class LoxCallable;
using Object = std::variant<std::monostate, double, std::string, bool, shared_ptr<LoxCallable>>;

class Expr {
public:
    virtual ~Expr() = default;
class Visitor {
public:
   virtual Object visitAssignExpr(class Assign* Expr) = 0;
   virtual Object visitBinaryExpr(class Binary* Expr) = 0;
   virtual Object visitCallExpr(class Call* Expr) = 0;
   virtual Object visitGroupingExpr(class Grouping* Expr) = 0;
   virtual Object visitLiteralExpr(class Literal* Expr) = 0;
   virtual Object visitLogicalExpr(class Logical* Expr) = 0;
   virtual Object visitUnaryExpr(class Unary* Expr) = 0;
   virtual Object visitVariableExpr(class Variable* Expr) = 0;
};

virtual Object accept(class Visitor* visitor) =0; 
};

class Assign : public Expr {
public:
    Assign(Token name, shared_ptr<Expr> value) : name(name), value(value) {}
    Token name;
    shared_ptr<Expr> value;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitAssignExpr(this);
}

};

class Binary : public Expr {
public:
    Binary(shared_ptr<Expr> left, Token operatorToken, shared_ptr<Expr> right) : left(left), operatorToken(operatorToken), right(right) {}
    shared_ptr<Expr> left;
    Token operatorToken;
    shared_ptr<Expr> right;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitBinaryExpr(this);
}

};

class Call : public Expr {
public:
    Call(shared_ptr<Expr> callee, Token paren, vector<shared_ptr<Expr>> arguments) : callee(callee), paren(paren), arguments(arguments) {}
    shared_ptr<Expr> callee;
    Token paren;
    vector<shared_ptr<Expr>> arguments;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitCallExpr(this);
}

};

class Grouping : public Expr {
public:
    Grouping(shared_ptr<Expr> expression) : expression(expression) {}
    shared_ptr<Expr> expression;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitGroupingExpr(this);
}

};

class Literal : public Expr {
public:
    Literal(Token value) : value(value) {}
    Token value;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitLiteralExpr(this);
}

};

class Logical : public Expr {
public:
    Logical(shared_ptr<Expr> left, Token operatorToken, shared_ptr<Expr> right) : left(left), operatorToken(operatorToken), right(right) {}
    shared_ptr<Expr> left;
    Token operatorToken;
    shared_ptr<Expr> right;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitLogicalExpr(this);
}

};

class Unary : public Expr {
public:
    Unary(Token operatorToken, shared_ptr<Expr> right) : operatorToken(operatorToken), right(right) {}
    Token operatorToken;
    shared_ptr<Expr> right;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitUnaryExpr(this);
}

};

class Variable : public Expr {
public:
    Variable(Token name) : name(name) {}
    Token name;
	Object accept (Expr::Visitor* visitor) override {
    return visitor->visitVariableExpr(this);
}

};


