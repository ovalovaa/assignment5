#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

class ASTNode;

class Context {
public:
    using BuiltinFunc = std::function<double(const std::vector<double>&)>;

    std::unordered_map<std::string, BuiltinFunc> builtins;
    std::unordered_map<std::string, double> variables;

    struct UserFunction {
        std::vector<std::string> params;
        std::unique_ptr<ASTNode> body;
    };

    std::unordered_map<std::string, UserFunction> user_functions;

    Context();

    double call_builtin(const std::string& name, const std::vector<double>& args) const;
    bool has_variable(const std::string& name) const;
    double get_variable(const std::string& name) const;
    void set_variable(const std::string& name, double value);
    void define_user_function(const std::string& name, std::vector<std::string> params, std::unique_ptr<ASTNode> body);
    double call_user_function(const std::string& name, const std::vector<double>& args);
};