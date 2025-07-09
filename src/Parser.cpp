#include <bits/stdc++.h>
//#include "Token.h"
#include "Expr.h"
#include "Stmt.h"

using namespace std;

class Parser {
    private:
        class ParseError : public exception {
            private:
                string message;
            public:
                ParseError(string message) : message(message) {}
                const char* what() const noexcept override {
                    return message.c_str();
                }
        };
        bool isAtEnd(){
            return peek().type == TokenType::END_OF_FILE;
        }
        Token peek(){
            return tokens[current];
        }
        Token previous(){
            return tokens[current-1];
        }
        runtime_error error(Token token, string message){
            if(token.type == TokenType::END_OF_FILE){
                return runtime_error("Error at end of file: "+ message);
            } else {
                return runtime_error("Error at " + to_string(token.line) + ": " + message + " at '" + token.lexeme + "'");
            }
        }
        void synchronize(){
            advance();
            while(!isAtEnd()){
                if(previous().type == TokenType::SEMICOLON) return;
                switch (peek().type){
                    case TokenType::CLASS:
                    case TokenType::FUNC:
                    case TokenType::FOR:
                    case TokenType::VAR:
                    case TokenType::IF:
                    case TokenType::WHILE:
                    case TokenType::PRINT:
                    case TokenType::RETURN:
                        return;
                    default:
                        // Do nothing, just advance
                        break;
                    }
                    advance(); 
                }
            }
        
        bool check(TokenType type){
            if (isAtEnd()) return false;
            return peek().type == type;
        }
        Token advance(){
            if(!isAtEnd()) current++;
            return previous();
        }
        bool match(vector<TokenType> types){
            for(TokenType type: types){
                if(check(type)){
                    advance();
                    return true;
                }
            }
            return false;
        }
        Token consume(TokenType type, string message){
            if (check(type)) return advance();
            throw error(peek(), message);
        }
        
        vector<Token> tokens;
        int current =0;
        

        shared_ptr<Expr> expression(){
            return assignment();
        }
        shared_ptr<Stmt> statement(){
            if (match({TokenType::FOR})){
                return forStatement();
            }
            if (match({TokenType::IF})){
                return ifStatement();
            }
            if (match({TokenType::PRINT})){
                return printStatement();
            }
            if (match({TokenType::WHILE})){
                return whileStatement();
            }
            if (match({TokenType::LEFT_BRACE})){
                return make_shared<Block>(block());
            }
            return expressionStatement();
        }
        shared_ptr<Stmt> declaration(){
            try{
                if (match({TokenType::FUNC})){
                    return function("function");
                }
                if (match({TokenType::VAR})) return varDeclaration();
                return statement();
            } catch (Parser::ParseError& e) {
                cerr << e.what() << endl;
                synchronize();
                return nullptr; // Return null if declaration fails
            }
        }

        shared_ptr<Stmt> varDeclaration(){
            Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
            shared_ptr<Expr> initializer = nullptr;
            if (match({TokenType::EQUAL})){
                initializer = expression();
            }
            consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
            return make_shared<Var>(name, initializer);
        }

        shared_ptr<Stmt> forStatement(){
            consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
            shared_ptr<Stmt> initializer;
            if (match({TokenType::SEMICOLON})){
                initializer = nullptr;
            } else if (match({TokenType::VAR})){
                initializer = varDeclaration();
            } else {
                initializer = expressionStatement();
            }
            shared_ptr<Expr> condition = nullptr;
            if (!check(TokenType::SEMICOLON)){
                condition = expression();
            }
            consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

            shared_ptr<Expr> increment = nullptr;
            if(!check(TokenType::RIGHT_PAREN)){
                increment = expression();
            }
            consume(TokenType::RIGHT_PAREN, "Expect ')' after for loop.");
            shared_ptr<Stmt> body = statement();

            // --- FIX: increment goes inside the loop body ---
            if (increment != nullptr){
                body = make_shared<Block>(vector<shared_ptr<Stmt>>{
                    body,
                    make_shared<Expression>(increment)
                });
            }
            if(condition == nullptr){
                condition = make_shared<Literal>(Token(TokenType::TRUE, "true", true, 0));
            }
            body = make_shared<While>(condition, body);

            if (initializer != nullptr){
                body = make_shared<Block>(vector<shared_ptr<Stmt>>{
                    initializer,
                    body
                });
            }
            return body;
        }

        shared_ptr<Stmt> whileStatement(){
            consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
            shared_ptr<Expr> condition = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
            shared_ptr<Stmt> body = statement();

            return make_shared<While>(condition, body);
        }

        shared_ptr<Stmt> ifStatement(){
            consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
            shared_ptr<Expr> condition = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

            shared_ptr<Stmt> thenBranch = statement();
            shared_ptr<Stmt> elseBranch = nullptr;
            if (match({TokenType::ELSE})){
                elseBranch = statement();
            }
            return make_shared<If>(condition, thenBranch, elseBranch);
        }

        shared_ptr<Stmt> printStatement(){
            shared_ptr<Expr> expr = expression();
            consume(TokenType::SEMICOLON, "Expect ';' after value.");
            return make_shared<Print>(expr);
        }

        shared_ptr<Stmt> expressionStatement(){
            shared_ptr<Expr> expr = expression();
            consume(TokenType::SEMICOLON, "Expect ';' after value.");
            return make_shared<Expression>(expr);
        }

        shared_ptr<Stmt> function(string kind){
            Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after "+kind+" name.");
            vector<Token> parameters;
            if (!check(TokenType::RIGHT_PAREN)){
                do{
                    if (parameters.size() >= 255){
                        error(peek(), "Can't have more than 255 parameters.");
                    }
                    parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
                } while(match({TokenType::COMMA}));
                }
                consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

                consume(TokenType::LEFT_BRACE, "Expect '{' before "+kind+" body.");
                vector<shared_ptr<Stmt>> body = block();
                return make_shared<Function>(name, parameters, body);
            }
            
        

        vector<shared_ptr<Stmt>> block() {
            vector<shared_ptr<Stmt>> statements;
            while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()){
                statements.push_back(declaration());
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
            return statements;
        }

        shared_ptr<Expr> assignment(){
            shared_ptr<Expr> expr = Oroperator();

            if (match({TokenType::EQUAL})){
                Token equals = previous();
                shared_ptr<Expr> value = assignment();

                if (dynamic_pointer_cast<Variable>(expr)){
                    Token name = dynamic_pointer_cast<Variable>(expr)->name;
                    return make_shared<Assign>(name, value);
                }
                throw error(equals, "Invalid assignment target.");
            }
            return expr;
        }// Checks if the current token is an assignment operator and if so, it processes the assignment.

        shared_ptr<Expr> Oroperator(){
            shared_ptr<Expr> expr = Andoperator();

            while(match({TokenType::OR})){
                Token operatorToken = previous();
                shared_ptr<Expr> right = Andoperator();
                expr = make_shared<Logical>(expr, operatorToken, right);
            }
            return expr;
        }

        shared_ptr<Expr> Andoperator(){
            shared_ptr<Expr> expr = equality();

            while (match({TokenType::AND})){
                Token operatorToken = previous();
                shared_ptr<Expr> right = equality();
                expr = make_shared<Logical>(expr, operatorToken, right);
            }
            return expr;    
        }

        shared_ptr<Expr> equality(){
            shared_ptr<Expr> expr = comparison();

            while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
                Token operatorToken = previous();
                shared_ptr<Expr> right = comparison();
                expr = make_shared<Binary>(expr,operatorToken, right);
            }
            return expr;
        }

        shared_ptr<Expr> comparison(){
            shared_ptr<Expr> expr = term();

            while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
                Token operatorToken = previous();
                shared_ptr<Expr> right = term();
                expr = make_shared<Binary>(expr,operatorToken, right);
            }
            return expr;
        }

        shared_ptr<Expr> term(){
            shared_ptr<Expr> expr = factor();

            while (match({TokenType::MINUS, TokenType::PLUS})) {
                Token operatorToken = previous();
                shared_ptr<Expr> right = factor();
                expr = make_shared<Binary>(expr,operatorToken, right);
            }
            return expr;
        }

        shared_ptr<Expr> factor(){
            shared_ptr<Expr> expr = unary();

            while (match({TokenType::SLASH, TokenType::STAR})) {
                Token operatorToken = previous();
                shared_ptr<Expr> right = unary();
                expr = make_shared<Binary>(expr,operatorToken, right);
            }
            return expr;
        }

        shared_ptr<Expr> unary(){
            if (match({TokenType::BANG, TokenType::MINUS})){
                Token operatorToken = previous();
                shared_ptr<Expr> right = unary();
                return make_shared<Unary>(operatorToken,right);
            }
            return call();
        }

        shared_ptr<Expr> finishCall(shared_ptr<Expr> callee){
            vector<shared_ptr<Expr>> arguments;
            if(!check(TokenType::RIGHT_PAREN)){
                do{
                    if(arguments.size() >= 255){
                        error(peek(), "Can't have more than 255 arguments.");
                    }
                    arguments.push_back(expression());
                }while (match({TokenType::COMMA}));              
            }
            Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            return make_shared<Call>(callee,paren,arguments);
        }

        shared_ptr<Expr> call(){
            shared_ptr<Expr> expr = primary();

            while(true){
                if(match({TokenType::LEFT_PAREN})){
                    expr = finishCall(expr);
                }else {
                    break;
                }
            }
            return expr;
        }

        shared_ptr<Expr> primary(){
            if (match({TokenType::FALSE})){
                return make_shared<Literal>(Token(TokenType::FALSE, "false", false, 0));
            }
            if (match({TokenType::TRUE})){
                return make_shared<Literal>(Token(TokenType::TRUE, "true", true, 0));
            }
            if (match({TokenType::NIL})){
                return make_shared<Literal>(Token(TokenType::NIL, "nil", monostate(), 0));
            }
            if (match({TokenType::NUMBER})){
                Token token = previous();
                return make_shared<Literal>(Token(TokenType::NUMBER, token.lexeme, get<double>(token.literal), token.line));
            }
            if (match({TokenType::STRING})){
                Token token = previous();
                return make_shared<Literal>(Token(TokenType::STRING, token.lexeme, get<string>(token.literal), token.line));
            }
            if (match({TokenType::IDENTIFIER})){
                return make_shared<Variable>(previous());
            }
            if (match({TokenType::LEFT_PAREN})){
                shared_ptr<Expr> expr = expression();
                consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
                return make_shared<Grouping>(expr);
            }
            throw error(peek(), "Expect expression.");
        }
        
    public:
        Parser(vector<Token> tokens){
            this->tokens = tokens;
        };

        vector<shared_ptr<Stmt>> parse(){
            try{
                vector<shared_ptr<Stmt>> statements;
                while(!isAtEnd()){
                    statements.push_back(declaration());
                }
                return statements;
            } catch (runtime_error& e) {
                cerr << e.what() << endl;
                synchronize();
                return {}; // Return empty vector if parsing fails
            }
        }
};