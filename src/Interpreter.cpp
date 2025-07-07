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
//#include "LoxCallable.h"
//using Object = variant<double, string, monostate>;

class Interpreter : public Expr::Visitor, public Stmt::Visitor {
     private:
          shared_ptr<Environment> globals;
          shared_ptr<Environment> environment;
     public:
           Interpreter() {
               globals = make_shared<Environment>();
               environment = globals;
          }

          shared_ptr<Environment> getGlobals() const { return globals; }
          shared_ptr<Environment> getEnvironment() const { return environment; }

          void executeBlock(vector<shared_ptr<Stmt>> statements, shared_ptr<Environment> Newenvironment){
               auto previous = this->environment; // Save the current environment
               this->environment = Newenvironment;
               try {
                    for(const auto& statement : statements){
                         execute(statement);
                    }
               }
               catch (RuntimeError& error) {
                    cerr << "Runtime error: " << error.what() << endl;
               }
               this->environment = previous; // Restore the previous environment
          }

          Object visitLiteralExpr(Literal* expr) override{
               return expr->value.literal;
          }
          Object visitLogicalExpr(Logical* expr) override{
               Object left = evaluate(expr->left);
               if (expr->operatorToken.type == TokenType::OR){
                    if(isTruthy(left)) return left;
               } else {
                    if(!isTruthy(left)) return left;
               }
               return evaluate(expr->right);
          }
          Object visitGroupingExpr(Grouping* expr) override{
               return evaluate(expr->expression);
          }
          Object visitUnaryExpr(Unary* expr)override{
               Object right = evaluate(expr->right);
               switch (expr->operatorToken.type){
                    case TokenType::BANG:
                         if(holds_alternative<bool>(right)){
                              return !isTruthy(right);
                         }
                    case TokenType::MINUS:
                         checkNumberOperand(expr->operatorToken, right);
                         if(holds_alternative<double>(right)){
                              return -get<double>(right);
                         }
                         return monostate();
               }
          }
          Object visitBinaryExpr(Binary* expr) override{
               Object left = evaluate(expr->left);
               Object right = evaluate(expr->right);
               
               switch (expr->operatorToken.type){
                    case TokenType::MINUS:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) - get<double>(right);
                         }
                    case TokenType::SLASH:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if (holds_alternative<double>(right) && get<double>(right) == 0) {
                         throw RuntimeError("You cannot divide by zero you Sick Bastard.");
                         }  
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) / get<double>(right);
                         }
                    case TokenType::STAR:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) * get<double>(right);
                         }
                    case TokenType::PLUS:
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) + get<double>(right);
                         }
                         if(holds_alternative<string>(left) && holds_alternative<string>(right)){
                              return get<string>(left)+ get<string>(right);
                         }
                         throw RuntimeError("Operands must be two numbers or two strings. Anything else is what disaster brings :");
                    case TokenType::GREATER:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) > get<double>(right);
                         }   
                    case TokenType::GREATER_EQUAL:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) >= get<double>(right);   
                         }
                    case TokenType::LESS:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) < get<double>(right);
                         }
                    case TokenType::LESS_EQUAL:
                         checkNumberOperands(expr->operatorToken, left, right);
                         if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                              return get<double>(left) <= get<double>(right);
                         }
                    case TokenType::BANG_EQUAL:
                         return !isEqual(left, right);
                    case TokenType::EQUAL_EQUAL:
                         return isEqual(left, right);
                    default:
                         cerr << "Unknown operator: " << expr->operatorToken.lexeme << endl;
                         return monostate();
          }
     }
          // Object visitCallExpr(Call* expr) override {
          //      Object callee = evaluate(expr->callee);
          //      vector<Object> arguments;
          //      for(shared_ptr<Expr> argument: expr->arguments){
          //           arguments.push_back(evaluate(argument));
          //      }
          //      if(!holds_alternative<LoxCallable>(callee)){
          //           throw RuntimeError("Can only call functions and classes.");
          //      }
          //      shared_ptr<LoxCallable> function = make_shared<LoxCallable>(callee);
          //      if(arguments.size() != function->arity()){
          //           throw RuntimeError("Expected " + to_string(function->arity()) + " arguments but got " + to_string(arguments.size()) + ".");
          //      }
          //      return function->call(this,arguments);
          // }
          Object visitVariableExpr(Variable* expr) override {
               return environment->get(expr->name.lexeme);
          }
          void interpret(vector<shared_ptr<Stmt>> statements){
               try{
                    for(shared_ptr<Stmt> statement : statements){
                         execute(statement);
                    }
               }
               catch(RuntimeError& error){
                    cerr << "Runtime error: " << error.what() << endl;
               }
          }

          Object visitBlockStmt(Block* stmt) override {
               executeBlock(stmt->statements, make_shared<Environment>(environment));
               return monostate(); // No return value for block statements
          }

          Object visitExpressionStmt(Expression* stmt)override {
               evaluate(stmt->expression);
               return monostate(); // No return value for expression statements
          }

          // Object visitFunctionStmt(Function* stmt) override {
          //      shared_ptr<LoxFunction> function = make_shared<LoxFunction>(stmt);
          //      environment->define(stmt->name.lexeme, function);
          //      return monostate(); // No return value for function declarations
          // }

          Object visitIfStmt(If* stmt)override {
               if (isTruthy(evaluate(stmt->condition))){
                    execute(stmt->thenBranch);
               } else if (stmt->elseBranch !=nullptr){
                    execute(stmt->elseBranch);
               }
               return monostate(); // No return value for if statements
          }
          Object visitPrintStmt(Print* stmt)override {
               Object value = evaluate(stmt->expression);
               cout << stringify(value) << endl;
               return monostate(); // No return value for print statements
          }
          Object visitVarStmt(Var* stmt) {
               Object value = monostate();
               if(stmt-> initializer !=nullptr){
                    value = evaluate(stmt->initializer);
               }
               environment->define(stmt->name.lexeme, value);
               return monostate(); // No return value for variable declarations
          }
          Object visitWhileStmt(While* stmt){
               while (isTruthy(evaluate(stmt->condition))) {
                    execute(stmt->body);
               }
               return monostate(); // No return value for while statements
          }
          Object visitAssignExpr(Assign* expr) override {
               Object value = evaluate(expr->value);
               environment->assign(expr->name.lexeme, value);
               return value; // Return the assigned value
          }

     private:
          
          // Helper methods for evaluating expressions and executing statements
          Object evaluate(shared_ptr<Expr> expr){
               return expr->accept(this);
          }
          void execute(shared_ptr<Stmt> stmt){
               stmt->accept(this);
          }
          
          bool isTruthy(Object object){
               if(holds_alternative<monostate>(object)) return false;
               if(holds_alternative<bool>(object))return get<bool>(object);
               if(holds_alternative<double>(object)) return get<double>(object) !=0;
               if(holds_alternative<string>(object)) return get<string>(object) != "";
          }
          bool isEqual(Object left, Object right){
               if(holds_alternative<monostate>(left) && holds_alternative<monostate>(right)) return true;
               if(holds_alternative<double>(left) && holds_alternative<double>(right)){
                    return get<double>(left) == get<double>(right);
               }
               if(holds_alternative<string>(left) && holds_alternative<string>(right)){
                    return get<string>(left) == get<string>(right);
               }
               if(holds_alternative<bool>(left) && holds_alternative<bool>(right)){
                    return get<bool>(left) == get<bool>(right);
               }
               return false;
          }
          void checkNumberOperand(Token operatorToken, Object operand){
               if(!holds_alternative<double>(operand)){
                    throw RuntimeError("Operand must be a number.");
               }
          }
          void checkNumberOperands(Token operatorToken, Object left, Object right){
               if(!holds_alternative<double>(left) || !holds_alternative<double>(right)){
                    throw RuntimeError("Operands must be numbers.");
               }
          }
          string stringify(Object object){
               if(holds_alternative<monostate>(object)) return "nil";
               if(holds_alternative<double>(object)) return to_string(get<double>(object));
               if(holds_alternative<string>(object)) return get<string>(object);
               if(holds_alternative<bool>(object)) return get<bool>(object) ? "true" : "false";
               return "unknown";
          }
};
