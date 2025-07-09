#include <string>
#include <iostream>
// #include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.cpp"
#include "Parser.cpp"
#include "AstPrinter.cpp"
#include "RuntimeError.h"
#include "Interpreter.h"
//#include "Token.h"
using namespace std;

Interpreter interpreter; // Create a global interpreter instance
// Global variables to track error states
bool hadError = false;
bool hadRuntimeError = false;

class Lox {
public:

    static void runFile(string path);
    static void runPrompt();
    static void run(string source);
    static void error(int line, string message);
    static bool hadError;
};
//function to run lox code
void run(string source){
    try {if (source.empty()) {
        cerr << "No source code provided.\n";
        return;
    }
    Scanner scanner = Scanner(source);
    vector<Token> tokens = scanner.scanTokens();
    Parser parser = Parser(tokens);
    vector<shared_ptr<Stmt>> statements = parser.parse();
    if (hadError) {
        cerr << "Parsing failed due to previous errors.\n";
        return;
    }
    interpreter.interpret(statements);
    }
    catch(RuntimeError& error) {
        cerr << "Runtime error: " << error.what() << endl;
        hadRuntimeError = true;
    }
}
//function to run a lox file
void runFile(string path){
    ifstream file(path);
    if(!file.is_open()){
        cerr << "couldnt open file: " <<path << "\n";
        exit(65);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
    if(hadError) {
        cerr << "Errors occurred during execution.\n";
        exit(65); // Exit code for error
    }
    if(hadRuntimeError) {
        cerr << "Runtime errors occurred.\n";
        exit(70);
    } // Exit code for runtime error
}

//function to run a lox prompt
void runPrompt(){
    string input;
    while (true){
        cout << "> ";
        getline(cin, input);
        if(input.empty()) continue;
        run(input);
        hadError = false; // Reset error state after each input
    }
}

void error(int line, string message) {
    cerr << "[line " << line << "] Error: " << message << "\n";
    hadError = true;
}
void runTimeError(RuntimeError& error) {
    cerr << "Runtime error: " << error.what() << endl;
    hadRuntimeError = true;
}

int main(int argc, char* argv[]){
    if(argc > 2){
        cout << "usage: clox [script]\n";
        return 64;
    }
    else if(argc ==2){
        runFile(argv[1]);
        if(hadError) {
            return 65; // Exit code for error
        }

    }else {
        runPrompt();
    }
    return 0;
}