#include "context.h"
#include "ast_nodes.h"
#include <cmath>
#include <stdexcept>

Context::Context() {
    builtins["pow"] = [](const std::vector<double>& a) { return std::pow(a[0], a[1]); };
    builtins["abs"] = [](const std::vector<double>& a) { return std::abs(a[0]); };
    builtins["max"] = [](const std::vector<double>& a) { return std::max(a[0], a[1]); };
    builtins["min"] = [](const std::vector<double>& a) { return std::min(a[0], a[1]); };
}

double Context::call_builtin(const std::string& name, const std::vector<double>& args) const {
    auto it = builtins.find(name);
    if (it == builtins.end()) throw std::runtime_error("Unknown builtin function: " + name);
    return it->second(args);
}

bool Context::has_variable(const std::string& name) const {
    return variables.count(name) > 0;
}

double Context::get_variable(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) throw std::runtime_error("Undefined variable: " + name);
    return it->second;
}

void Context::set_variable(const std::string& name, double value) {
    variables[name] = value;
}

void Context::define_user_function(const std::string& name, std::vector<std::string> params, std::unique_ptr<ASTNode> body) {
    user_functions[name] = {params, std::move(body)};
}

double Context::call_user_function(const std::string& name, const std::vector<double>& args) {
    auto it = user_functions.find(name);
    if (it == user_functions.end()) throw std::runtime_error("Unknown user function: " + name);
    if (args.size() != it->second.params.size()) throw std::runtime_error("Argument count mismatch");

    Context local_ctx;
    local_ctx.builtins = builtins;
    for (size_t i = 0; i < args.size(); ++i)
        local_ctx.variables[it->second.params[i]] = args[i];

    return it->second.body->evaluate(local_ctx);
}
