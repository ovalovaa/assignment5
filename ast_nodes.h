#pragma once
#include <string>
#include <memory>
#include <vector>

class Context;

class ASTNode {
public:
    virtual double evaluate(Context& ctx) const = 0;
    virtual std::unique_ptr<ASTNode> clone() const = 0;
    virtual ~ASTNode() = default;
};

class NumberNode : public ASTNode {
    double value;
public:
    explicit NumberNode(double v) : value(v) {}
    double evaluate(Context&) const override { return value; }
    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<NumberNode>(value);
    }
};

class BinaryOpNode : public ASTNode {
    char op;
    std::unique_ptr<ASTNode> left, right;
public:
    BinaryOpNode(char o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    double evaluate(Context& ctx) const override;
    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<BinaryOpNode>(op, left->clone(), right->clone());
    }
};

class FunctionCallNode : public ASTNode {
    std::string func_name;
    std::vector<std::unique_ptr<ASTNode>> args;
public:
    FunctionCallNode(std::string name, std::vector<std::unique_ptr<ASTNode>> a)
        : func_name(std::move(name)), args(std::move(a)) {}
    double evaluate(Context& ctx) const override;
    std::unique_ptr<ASTNode> clone() const override {
        std::vector<std::unique_ptr<ASTNode>> copied;
        for (const auto& arg : args)
            copied.push_back(arg->clone());
        return std::make_unique<FunctionCallNode>(func_name, std::move(copied));
    }
};

class VariableNode : public ASTNode {
    std::string name;
public:
    explicit VariableNode(std::string n) : name(std::move(n)) {}
    double evaluate(Context& ctx) const override;
    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<VariableNode>(name);
    }
};

class AssignmentNode : public ASTNode {
    std::string var_name;
    std::unique_ptr<ASTNode> expr;
public:
    AssignmentNode(std::string name, std::unique_ptr<ASTNode> e)
        : var_name(std::move(name)), expr(std::move(e)) {}
    double evaluate(Context& ctx) const override;
    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<AssignmentNode>(var_name, expr->clone());
    }
};

class UserFunctionDefNode : public ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<ASTNode> body;
public:
    UserFunctionDefNode(std::string n, std::vector<std::string> p, std::unique_ptr<ASTNode> b)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)) {}
    double evaluate(Context& ctx) const override;
    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<UserFunctionDefNode>(name, params, body->clone());
    }
};