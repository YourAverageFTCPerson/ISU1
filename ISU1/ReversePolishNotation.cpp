#include "ReversePolishNotation.h"

#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>

#include "utils.h"

using namespace ReversePolishNotation;

TokenType Operand::getType() { return TokenType::OPERAND; }

Operand::Operand(double value) : value{value} {}

Operand::Operand() : value{0.0} {}

std::ostream& operator<<(std::ostream& os, const Operand& o) {
    os << "ReversePolishNotation::Operand[value=" << o.value << ']';
    return os;
}

Operator::Operator() {}

Operator::Operator(int numArgs, double (*callback)(double args[], size_t amount))
    : numArgs(numArgs), callback{callback} {}

Operator::Operator(const Operator& op) : numArgs{ op.numArgs }, callback{ op.callback } {
}

TokenType Operator::getType() { return TokenType::OPERATOR; }

static double* getLastOperands(std::deque<double>& operands, int amount) {
    MAXWELL_ASSERT_FALSE(amount < 0, std::invalid_argument);

    double* result = new double[amount];
    for (size_t i = 0; i < amount; i++) {
        result[i] = operands.front();
        operands.pop_front();
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, TokenType type) {
    switch (type) {
        case TokenType::OPERAND:
            os << "TokenType::OPERAND";
            break;
        case TokenType::OPERATOR:
            os << "TokenType::OPERATOR";
            // break;
    }
    return os;
}

double ReversePolishNotation::evaluate(const std::vector<Token*> &tokens, double x, double y) {
    /*
    std::cout << "tokens.size(): " << tokens.size() << std::endl;
    for (auto *x : tokens) {
        if (x->getType() == TokenType::OPERAND) {
            std::cout << static_cast<Operand*>(x)->value << std::endl;
        } else {
            std::cout << "OPP" << std::endl;
        }
    }
    */
    std::deque<double> operands;

    for (size_t i = 0; i < tokens.size(); i++) {
        Token* token = tokens[i];
        switch (token->getType()) {
        case TokenType::OPERAND: {
            Operand *op = static_cast<Operand*>(token);
            if (op->x)
                operands.push_front(x);
            else if (op->y)
                operands.push_front(y);
            else
                operands.push_front(op->value);
        } break;
        case TokenType::OPERATOR: {
            auto* op = static_cast<Operator*>(token);
            int numArgs = op->numArgs;
            auto* pointer = getLastOperands(operands, numArgs);
            operands.push_front(op->callback(pointer, numArgs));
            delete[] pointer;
        }  // break;
        }
    }

    //std::cout << operands.size() << std::endl;

    MAXWELL_ASSERT_FALSE(operands.size() != 1, std::invalid_argument);
    return operands.front();
}

//double add(double args[]) { return args[1] + args[0]; }

// #undef MAXWELL_ASSERT_FALSE

// int main()
// {
//     Operand a = Operand(1.0), b = Operand(2.0);
//     Operator c = Operator(2, add);
//     Token *tokens[] = {&a, &b, &c};
//     std::cout << "Result: ";
//     std::cout << ReversePolishNotation::evaluate(tokens, 3) << std::endl;
//     return 0;
// }
