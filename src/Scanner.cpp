//include <bits/stdc++.h>
//#include "TokenType.h"
#include "Token.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;

class Scanner {
    public:
        string source;
        vector<Token> tokens;
        int start =0;
        int current = 0;
        int line = 1;

        Scanner(string source) : source(source) {}

        vector<Token> scanTokens() {
            while(!isAtEnd()){
                start = current;
                scanToken();
            }
            tokens.push_back(Token(TokenType::END_OF_FILE, "", monostate(), line));
            return tokens;
        }

        unordered_map<string, TokenType> keywords = {
            {"and", TokenType::AND},
            {"class", TokenType::CLASS},
            {"else", TokenType::ELSE},
            {"false", TokenType::FALSE},
            {"for", TokenType::FOR},
            {"fun", TokenType::FUNC},
            {"if", TokenType::IF},
            {"nil", TokenType::NIL},
            {"or", TokenType::OR},
            {"print", TokenType::PRINT},
            {"return", TokenType::RETURN},
            {"super", TokenType::SUPER},
            {"this", TokenType::THIS},
            {"true", TokenType::TRUE},
            {"var", TokenType::VAR},
            {"while", TokenType::WHILE}
        };

        bool isAtEnd(){
            return current >= static_cast<int>(source.length());
            //returns true if current is at the end of the source string
        }
        bool isDigit(char c){
            return c >= '0' && c <= '9';
            //returns true if character is a digit
        }
        bool isAlpha(char c){
            return (c>= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
            //returns true if character is an alphabet or underscore
        }
        bool isAlphaNumeric(char c){
            return isAlpha(c) || isDigit(c);
            //returns true if character is alphanumeric 
        }
        void scanToken(){
            char c = advance();
            switch (c)
            {
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            
            
                case '\n':
                line++; break;
            case '(':
                addToken(TokenType::LEFT_PAREN); break;
            case ')':
                addToken(TokenType::RIGHT_PAREN); break;
            case '{':
                addToken(TokenType::LEFT_BRACE); break;
            case '}':
                addToken(TokenType::RIGHT_BRACE); break;
            case ',':
                addToken(TokenType::COMMA); break;
            case '.':
                addToken(TokenType::DOT); break;
            case '-':
                addToken(TokenType::MINUS); break;
            case '+':
                addToken(TokenType::PLUS); break;
            case ';':
                addToken(TokenType::SEMICOLON); break;
            case '*':
                addToken(TokenType::STAR); break;
            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL:TokenType::BANG); break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
            case '/':
                if(match('/')) {
                    // A comment goes until the end of the line.
                    while(lookahead() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;  
                
            case '"': stringify(); break;  
                default:
                if(isDigit(c)){
                    numberify();
                }
                else if (isAlpha(c)) {
                    identifier();
                }
                else {
                    cerr << "[Line " << line << "] Error: Unexpected character: " << c << endl;
                }
                break;
            }

        }

        //Utility functions
        char advance() {
            return source[current++];
            //returns current character and increments current
        }
        bool match(char expected){
            if(isAtEnd()) return false;
            if(source[current] != expected) return false;
            current++;
            return true;
            //retuns true if next character is same as expected
        }
        char lookahead() {
            if(isAtEnd()) return '\0';
            return source[current];
            //returns next character without incrementing current
        }
        char lookahead_next(){
            if(current+1 >= static_cast<int>(source.length())) return '\0';
            return source[current+1];
            //returns the character after the next one without incrementing current
        }
        void addToken(TokenType type){
            addToken(type, monostate());
            // Adds a token with no literal value
        }
        void addToken(TokenType type, variant<monostate, double,string, bool> literal){
            string text = source.substr(start, current-start);
            tokens.push_back(Token(type, text, literal, line));
            //adds a token to the tokens vector
        }

        //Type Functions
        void stringify() {
            while(lookahead() != '"' && !isAtEnd()){
                if(lookahead()== '\n') line++;
                advance();
            }
            if(isAtEnd()){
                cerr << "[Line " << line << "]* Error: Unterminated String." <<endl;
                return;
            }
            advance(); //close the string with '"'
            string value = source.substr(start+1, current - start - 2);
            addToken(TokenType::STRING, value);
        }
        void numberify(){
            while(isDigit(lookahead())) advance();
            //Loop to consume all digits
            if(lookahead() == '.' && isDigit(lookahead_next())){
                advance(); //consume the '.'
                while(isDigit(lookahead())) advance();
                //Loop to consume all digits after the '.'  
            }
            addToken(TokenType::NUMBER, stod(source.substr(start, current-start)));
        }
        void identifier(){
            while(isAlphaNumeric(lookahead())){
                advance();
            }
            string text = source.substr(start,current-start);
            TokenType type = TokenType::IDENTIFIER;
            if(keywords.find(text) != keywords.end()){
                type= keywords[text];
            }

            addToken(type);
        }


};