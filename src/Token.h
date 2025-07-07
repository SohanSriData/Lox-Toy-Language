#include "TokenType.h"
#include <string>
#include <variant>
#pragma once
using namespace std;

class Token {
    public:
        TokenType type;
        string lexeme;
        variant<monostate,double, string, bool> literal;
        int line;

        // Constructor for Token class
    Token(TokenType type, string lexeme, variant<monostate, double, string, bool> literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    string toString() const {
        string literalString;
        if(holds_alternative<string>(literal)){
            literalString = get<string>(literal);
        } 
        else if(holds_alternative<double>(literal)){
            literalString = to_string(get<double>(literal));
        }
        else {
            literalString = "nil"; // Handle the case for monostate
        }
        return "Token(" + to_string(type) + ", " + lexeme + ", " + literalString + ", " + to_string(line) + ")";
    }

};