// #include "LoxCallable.h"
// using namespace std;

// class LoxFunction : public LoxCallable {
//     private:
//         shared_ptr<Function> declaration;
//         LoxFunction(shared_ptr<Function> declaration){
//             this->declaration = declaration;
//         }

//     public:
//         Object call(Interpreter* interpreter, vector<Object> arguments) {
        
//         Environment environment(interpreter->getGlobals());
//         for(int i=0; i< declaration->params.size(); i++){
//             environment.define(declaration->params[i].lexeme, arguments[i]);
//         }
//         interpreter->executeBlock(declaration->body, make_shared<Environment>(environment));
//         return monostate();
//         }
//         size_t arity() const override {
//             return declaration->params.size();
//         }
//         string toString(){
//             return "<function " + declaration->name.lexeme + ">";
//         }
// };
