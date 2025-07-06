#include "parser.h"
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <algorithm>

// Обрізка пробілів
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// Перевірка чи вся строка — число
bool is_number(const std::string& s) {
    char* end = nullptr;
    std::strtod(s.c_str(), &end);
    return end == s.c_str() + s.size();
}

// Парсинг з урахуванням пріоритетів операцій
std::unique_ptr<ASTNode> parse_expression(const std::string& input) {
    std::string expr = trim(input);

    // Підтримка "var a = ..."
    if (expr.substr(0, 4) == "var ") {
        expr = trim(expr.substr(4));
    }

    // Визначення функції
    if (expr.substr(0, 4) == "def ") {
        size_t name_start = 4;
        size_t paren_open = expr.find('(', name_start);
        size_t paren_close = expr.find(')', paren_open);
        std::string func_name = trim(expr.substr(name_start, paren_open - name_start));
        std::string param_list = expr.substr(paren_open + 1, paren_close - paren_open - 1);
        std::vector<std::string> params;
        std::stringstream ss(param_list);
        std::string param;
        while (std::getline(ss, param, ','))
            params.push_back(trim(param));

        size_t brace_open = expr.find('{', paren_close);
        size_t brace_close = expr.find_last_of('}');
        std::string body_expr = expr.substr(brace_open + 1, brace_close - brace_open - 1);

        return std::make_unique<UserFunctionDefNode>(func_name, params, parse_expression(body_expr));
    }

    // Присвоєння змінної
    if (expr.find('=') != std::string::npos && expr.find("==") == std::string::npos) {
        size_t eq = expr.find('=');
        std::string name = trim(expr.substr(0, eq));
        std::string value = trim(expr.substr(eq + 1));
        return std::make_unique<AssignmentNode>(name, parse_expression(value));
    }

    // Арифметичні вирази: + -
    int parens = 0;
    for (int i = expr.size() - 1; i >= 0; --i) {
        if (expr[i] == ')') ++parens;
        else if (expr[i] == '(') --parens;
        else if ((expr[i] == '+' || expr[i] == '-') && parens == 0) {
            std::string left = expr.substr(0, i);
            std::string right = expr.substr(i + 1);
            return std::make_unique<BinaryOpNode>(expr[i], parse_expression(left), parse_expression(right));
        }
    }

    // Арифметичні вирази: * /
    parens = 0;
    for (int i = expr.size() - 1; i >= 0; --i) {
        if (expr[i] == ')') ++parens;
        else if (expr[i] == '(') --parens;
        else if ((expr[i] == '*' || expr[i] == '/') && parens == 0) {
            std::string left = expr.substr(0, i);
            std::string right = expr.substr(i + 1);
            return std::make_unique<BinaryOpNode>(expr[i], parse_expression(left), parse_expression(right));
        }
    }

    // Число
    if (is_number(expr)) {
        return std::make_unique<NumberNode>(std::stod(expr));
    }

    // Виклик функції
    if (expr.find('(') != std::string::npos && expr.back() == ')') {
        size_t lpar = expr.find('(');
        size_t rpar = expr.rfind(')');
        std::string func = trim(expr.substr(0, lpar));
        std::string args = expr.substr(lpar + 1, rpar - lpar - 1);

        std::vector<std::unique_ptr<ASTNode>> parsed_args;
        std::string current;
        int depth = 0;
        for (char c : args) {
            if (c == ',' && depth == 0) {
                parsed_args.push_back(parse_expression(trim(current)));
                current.clear();
            } else {
                if (c == '(') ++depth;
                if (c == ')') --depth;
                current += c;
            }
        }
        if (!current.empty()) {
            parsed_args.push_back(parse_expression(trim(current)));
        }

        return std::make_unique<FunctionCallNode>(func, std::move(parsed_args));
    }

    // Змінна
    return std::make_unique<VariableNode>(expr);
}
