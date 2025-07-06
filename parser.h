#pragma once
#include <string>
#include <memory>
#include "ast_nodes.h"

std::unique_ptr<ASTNode> parse_expression(const std::string& input);
