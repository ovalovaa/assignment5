#include "parser.h"
#include "context.h"
#include <iostream>

int main() {
    std::string input;
    Context ctx;
    std::cout << "> ";
    while (std::getline(std::cin, input)) {
        try {
            auto tree = parse_expression(input);
            double result = tree->evaluate(ctx);
            std::cout << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        std::cout << "> ";
    }
    return 0;
}
