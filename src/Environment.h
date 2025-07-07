#pragma once
#include <unordered_map>
using namespace std;
#include <variant> 
#include <string>
#include "RuntimeError.h"
using Object = variant<monostate, double, string, bool>;

class Environment {
    private:
        unordered_map<string, Object> values ;
    
    public:
        shared_ptr<Environment> enclosing; // For nested environments

        void define(string name, Object value) {
            values[name] = value;
        }
        Object get(string name){
            if(values.find(name) != values.end()){
                return values[name];
            }
            if(enclosing != nullptr) {
                return enclosing->get(name);
            } else {
                throw RuntimeError("Undefined variable '" + name + "'.");
            }
        }
        void assign(string name, Object value) {
            if(values.find(name) != values.end()){
                values[name] = value;
                return;
            }
            if(enclosing != nullptr){
                enclosing->assign(name, value);
                return;
            } else {
                throw RuntimeError("Undefined variable '" + name + "'.");
            }
        }
        
        Environment(){
            enclosing = nullptr; // Default to no enclosing environment
        }
        Environment(shared_ptr<Environment> enclosing) : enclosing(enclosing) {} // Constructor for nested environments
    

};