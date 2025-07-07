#include "Expr.h"
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <iostream>
#include <variant>  
using Object = std::variant<std::monostate, double, std::string, bool>;
// AstPrinter.h contents
class AstPrinter : Expr::Visitor {
public:
    std::string print(std::shared_ptr<Expr> expr){
    if (!expr) {
        return "<null expr>";
    }
    oss.str(""); 
    oss.clear();
    expr->accept(this);
    return oss.str();
};

    Object visitBinaryExpr(Binary* expr) override{
    return parenthesize(expr->operatorToken.lexeme, {expr->left, expr->right});
}

    Object visitGroupingExpr(Grouping* expr) override{
    return parenthesize("group", {expr->expression});
}

    Object visitLiteralExpr(Literal* expr) override{
    // Assuming Token.literal is std::variant<double, std::string>
    if (std::holds_alternative<double>(expr->value.literal)) {
        oss << std::get<double>(expr->value.literal);
    } else if (std::holds_alternative<std::string>(expr->value.literal)) {
        oss << std::get<std::string>(expr->value.literal);
    } else {
        oss << "nil";
    }
    return oss.str();
}
    Object visitUnaryExpr(Unary* expr) override{
    return parenthesize(expr->operatorToken.lexeme, {expr->right});
};

private:
    string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
    oss << "(" << name;
    for (const auto& expr : exprs) {
        oss << " ";
        expr->accept(this);
    }
    oss << ")";
    return oss.str();
}
    std::ostringstream oss;
};

// Usage example
// #include "AstPrinter.h"
// // 
// int main(){
//     AstPrinter printer;
//     std::cout << printer.print(std::make_shared<Binary>(
//         std::make_shared<Unary>(Token(TokenType::MINUS, "-", {}, 1), std::make_shared<Literal>(Token(TokenType::NUMBER, "123", 123.0, 1))),
//         Token(TokenType::STAR, "*", {}, 1),
//         std::make_shared<Grouping>(std::make_shared<Literal>(
//             Token(TokenType::NUMBER, "45.67", 45.67, 1)))
//     )) << std::endl; 
// }