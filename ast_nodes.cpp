#include "ast_nodes.h"
#include "context.h"
#include <stdexcept>

double VariableNode::evaluate(Context& ctx) const {
    return ctx.get_variable(name);
}

double AssignmentNode::evaluate(Context& ctx) const {
    double val = expr->evaluate(ctx);
    ctx.set_variable(var_name, val);
    return val;
}

double FunctionCallNode::evaluate(Context& ctx) const {
    std::vector<double> evaluated_args;
    for (const auto& arg : args)
        evaluated_args.push_back(arg->evaluate(ctx));

    if (ctx.builtins.count(func_name))
        return ctx.call_builtin(func_name, evaluated_args);
    else
        return ctx.call_user_function(func_name, evaluated_args);
}

double UserFunctionDefNode::evaluate(Context& ctx) const {
    ctx.define_user_function(name, params, body->clone());
    return 0;
}

double BinaryOpNode::evaluate(Context& ctx) const {
    double l = left->evaluate(ctx);
    double r = right->evaluate(ctx);
    switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return r != 0 ? l / r : throw std::runtime_error("Division by zero");
        default: throw std::runtime_error("Unknown operator");
    }
}
