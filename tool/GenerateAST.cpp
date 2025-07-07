#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>

using namespace std;

// Trim whitespace from both ends
string trim(const string& str) {
    size_t first = 0;
    while (first < str.size() && isspace(str[first])) ++first;

    size_t last = str.size();
    while (last > first && isspace(str[last - 1])) --last;

    return str.substr(first, last - first);
}

// Split by delimiter
vector<string> split(const string& str, const string& delim) {
    vector<string> tokens;
    size_t start = 0, end;
    while ((end = str.find(delim, start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delim.length();
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

void defineType(ofstream& writer, const string& baseName, const string& className, const string& fieldList) {
    writer << "class " << className << " : public " << baseName << " {\n";
    writer << "public:\n";

    // Split fields
    vector<string> fields = split(fieldList, ",");

    // Constructor signature
    writer << "    " << className << "(" << fieldList << ") : ";

    // Member initializer list
    for (size_t i = 0; i < fields.size(); ++i) {
        string field = trim(fields[i]);
        string name = split(field, " ")[1];
        writer << name << "(" << name << ")";
        if (i != fields.size() - 1) writer << ", ";
    }
    writer << " {}\n";

    // Member variables declaration
    for (const string& field : fields) {
        string trimmed = trim(field);
        vector<string> parts = split(trimmed, " ");
        string type = parts[0];
        string name = parts[1];
        writer << "    " << type << " " << name << ";\n";
    }

    

    writer << "\tObject accept (" << baseName << "::Visitor* visitor) override {\n";
    writer << "    return visitor->visit" << className << baseName << "(this);\n";
    writer << "}\n\n";
    writer << "};\n\n";
}

void defineVisitor(ofstream& writer, const string& baseName, const vector<string>& types){
    writer << "class Visitor {\n";
    writer << "public:\n";
    for (const string& type : types){
        string typeName = split(type, ":")[0];
        string trimmedtypeName = trim(typeName);
        writer << "   virtual Object visit" << trimmedtypeName <<baseName<<"(class " <<trimmedtypeName<< "* " << baseName << ")" << " = 0;\n";
    }
    writer << "};\n\n";
}

void defineAst(const string& outputDir, const string& baseName, const vector<string>& types) {
    string path = outputDir + "/" + baseName + ".h";
    ofstream writer(path);

    if (!writer.is_open()) {
        cerr << "Could not open file: " << path << "\n";
        exit(1);
    }
    if(baseName == "Stmt") writer << "#include \"Expr.h\"\n";
    writer << "#pragma once\n";
    writer << "#include <memory>\n";
    writer << "#include \"Token.h\"\n";
    writer << "using namespace std;\n\n";
    writer << "using Object = std::variant<std::monostate, double, std::string, bool>;\n\n";
    writer << "class " << baseName << " {\n";
    writer << "public:\n";
    writer << "    virtual ~" << baseName << "() = default;\n";
    defineVisitor(writer,baseName, types);
    writer << "virtual Object accept(class Visitor* visitor) =0; \n";
    writer << "};\n\n";

    for (const string& type : types) {
        size_t colonPos = type.find(':');
        string className = trim(type.substr(0, colonPos));
        string fields = trim(type.substr(colonPos + 1));
        defineType(writer, baseName, className, fields);
    }
    writer << "\n"; // Add a newline at the end of the file
}

// Main entry
int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: Generate_AST <output_dir>\n";
        return 64;
    }

    string outputDir = argv[1];

    defineAst(outputDir, "Expr", {
        "Assign   : Token name, shared_ptr<Expr> value",
        "Binary   : shared_ptr<Expr> left, Token operatorToken, shared_ptr<Expr> right",
    //    "Call     : shared_ptr<Expr> callee, Token paren, vector<shared_ptr<Expr>> arguments",
        "Grouping : shared_ptr<Expr> expression",
        "Literal  : Token value",
        "Logical : shared_ptr<Expr> left, Token operatorToken, shared_ptr<Expr> right",
        "Unary    : Token operatorToken, shared_ptr<Expr> right",
        "Variable : Token name",
    });
    
    defineAst(outputDir, "Stmt", {
        "Block : vector<shared_ptr<Stmt>> statements",
        "Expression : shared_ptr<Expr> expression",
    //    "Function : Token name, vector<Token> params, vector<shared_ptr<Stmt>> body",
        "If : shared_ptr<Expr> condition, shared_ptr<Stmt> thenBranch, shared_ptr<Stmt> elseBranch",
        "Print : shared_ptr<Expr> expression",
        "Var : Token name, shared_ptr<Expr> initializer",
        "While : shared_ptr<Expr> condition, shared_ptr<Stmt> body",
    });

    return 0;
}