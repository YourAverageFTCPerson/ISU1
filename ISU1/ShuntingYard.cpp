#include "ShuntingYard.h"

#include <cctype>
#include <iostream>

#include <numbers>

#include "utils.h"

static double divide(double args[], size_t amount) { return args[1] / args[0]; }

static double multiply(double args[], size_t amount) {
    return args[1] * args[0];
}

static double add(double args[], size_t amount) { return args[1] + args[0]; }

static double subtract(double args[], size_t amount) {
    return args[1] - args[0];
}

static double negative(double args[], size_t amount) { return -args[0]; }

static double positive(double args[], size_t amount) { return args[0]; }

static double pow(double args[], size_t amount) {
    return std::pow(args[1], args[0]);
}

static double exp(double args[], size_t amount) { return std::exp(args[0]); }

static double max(double args[], size_t amount) {
    return std::max(args[1], args[0]);
}

static double min(double args[], size_t amount) {
    return std::min(args[1], args[0]);
}

static double add_3(double args[], size_t amount) {
    return args[2] + args[1] + args[0];
}

static double V_cone(double args[], size_t amount) {
    return std::numbers::pi * args[0] * args[0] * args[1]; // pi * r^2 * h
}

static std::unordered_map<std::string, ReversePolishNotation::Operator>
    actualFunctions{{"exp", ReversePolishNotation::Operator(1, exp)},
                    {"max", ReversePolishNotation::Operator(2, max)},
                    {"min", ReversePolishNotation::Operator(2, min)},
                    {"add_3", ReversePolishNotation::Operator(3, add_3)},
                    {"V_cone", ReversePolishNotation::Operator(2, V_cone)}};

static ReversePolishNotation::Operator NEGATIVE(1, negative),
    POSITIVE(1, positive);

static std::unordered_map<char, ReversePolishNotation::Operator>
    actualOperators{{'/', ReversePolishNotation::Operator(2, divide)},
                    {'*', ReversePolishNotation::Operator(2, multiply)},
                    {'+', ReversePolishNotation::Operator(2, add)},
                    {'-', ReversePolishNotation::Operator(2, subtract)},
                    {'^', ReversePolishNotation::Operator(2, pow)}};

static std::unordered_map<double, ReversePolishNotation::Operand> numbers{};

static ReversePolishNotation::Operand generateX() {
    ReversePolishNotation::Operand x{};
    x.x = true;
    return x;
}

ReversePolishNotation::Operand x = generateX();

static ReversePolishNotation::Operand generateY() {
    ReversePolishNotation::Operand y{};
    y.y = true;
    return y;
}

ReversePolishNotation::Operand y = generateY();

static std::vector<ReversePolishNotation::Token *> convertSymbolsToTokens(
    std::deque<ShuntingYard::Symbol> symbols) {
    std::vector<ReversePolishNotation::Token *> result{symbols.size()};

    for (size_t i = 0; i < symbols.size(); i++) {
        switch (symbols[i].type) {
            case ShuntingYard::Symbol::Type::LITERAL_NUMERIC: {
                // ReversePolishNotation::Operand *operand =
                //     new ReversePolishNotation::Operand(
                //         std::stod(symbols[i].symbol));
                double value = std::stod(symbols[i].symbol);
                if (numbers.contains(value))
                    result[i] = &numbers[value];
                else {
                    ReversePolishNotation::Operand operand{value};
                    numbers[value] = operand;
                    result[i] = &numbers[value];
                }
            } break;
            case ShuntingYard::Symbol::Type::OPERATOR: {
                if (symbols[i].op.arguments == 1) {
                    switch (symbols[i].symbol[0]) {
                        case '+':
                            result[i] = &POSITIVE;
                            continue;
                        case '-':
                            result[i] = &NEGATIVE;
                            continue;
                    }
                }
                result[i] = &actualOperators[symbols[i].symbol[0]];
            } break;
            case ShuntingYard::Symbol::Type::FUNCTION:
                result[i] = &actualFunctions[symbols[i].symbol];
                break;
            case ShuntingYard::Symbol::Type::X:
                result[i] = &x;
                break;
            case ShuntingYard::Symbol::Type::Y:
                result[i] = &y;
                break;
            default:
                throw std::runtime_error(
                    "Can't convert ShuntingYard::Symbol to "
                    "ReversePolishNotation::Token");
        }
    }

    return result;
}

static std::unordered_map<char, ShuntingYard::Operator> operators{
    {'/', {2, 2}},
    {'*', {2, 2}},
    {'+', {1, 2}},
    {'-', {1, 2}},
    {'^', {1001, 2, true}}};

// List of functions for the Shunting Yard algorithm
static std::unordered_map<std::string, ShuntingYard::Function> functions{
    {"exp", {1}}, {"max", {2}}, {"min", {2}}, {"add_3", {3}}, {"V_cone", {2}} };

static const ShuntingYard::Symbol COMMA_TOKEN{
    ",", ShuntingYard::Symbol::Type::COMMA, 0, 0, false};

std::vector<ReversePolishNotation::Token *> ShuntingYard::convertToRPN(
    const std::string &expression) {
    //expression = removeWhitespace(expression);  // It's a copy so we can do this

    std::deque<Symbol> stkHolding;
    std::deque<Symbol> stkOutput;

    Symbol previous{"", Symbol::Type::UNKNOWN, 0, 0};

    char c;
    // for (const char c :
    //      expression) {  // TODO: turn this back into a regular for loop
    for (size_t i = 0; i < expression.size(); i++) {
        if (std::isdigit(c = expression[i]) || c == '.') {
            size_t j = i + 1;
            for (; j < expression.size(); j++)
                if (!std::isdigit(expression[j]) && expression[j] != '.') break;

            stkOutput.push_back(
                { expression.substr(i, j - i), Symbol::Type::LITERAL_NUMERIC });
            previous = stkOutput.back();

            // One less because i++ is at the end of every iteration
            i = j - 1;
        } else if (std::isspace(c)) {
            // do nothing
        } else if (c == 'x') {
            stkOutput.push_back({ "x", Symbol::Type::X });
            previous = stkOutput.back();
        } else if (c == 'y') {
            stkOutput.push_back({ "y", Symbol::Type::Y });
            previous = stkOutput.back();
        } else if (c == '(') {
            stkHolding.push_front(
                {std::string(1, c), Symbol::Type::PARENTHESIS_OPEN});
            previous = stkHolding.front();
        } else if (c == ')') {
            while (!stkHolding.empty() &&
                   stkHolding.front().type != Symbol::Type::PARENTHESIS_OPEN) {
                stkOutput.push_back(stkHolding.front());
                stkHolding.pop_front();
            }

            if (stkHolding.empty())
                throw std::invalid_argument("Unexpected parenthesis");

            MAXWELL_ASSERT_FALSE(
                stkHolding.front().type != Symbol::Type::PARENTHESIS_OPEN,
                std::runtime_error);
            stkHolding.pop_front();
            if (stkHolding.front().type == Symbol::Type::FUNCTION) {
                stkOutput.push_back(stkHolding.front());
                stkHolding.pop_front();
            }
            previous = {std::string(1, c),
                        Symbol::Type::PARENTHESIS_CLOSE};  // For [] later
        } else if (operators.contains(c)) {  // Remember to use C++ 20
            Operator newOp = operators[c];

            if (c == '-' || c == '+') {
                if (previous.type != Symbol::Type::LITERAL_NUMERIC && previous.type != Symbol::Type::X && previous.type != Symbol::Type::Y &&
                    previous.type != Symbol::Type::PARENTHESIS_CLOSE) {
                    newOp.arguments = 1;
                    newOp.precedence = 1000;
                }
            }

            while (!stkHolding.empty() &&
                   stkHolding.front().type != Symbol::Type::PARENTHESIS_OPEN && (stkHolding.front().op.precedence > newOp.precedence || (newOp.precedence == stkHolding.front().op.precedence && !stkHolding.front().op.rightAssociative))) {
                if (stkHolding.front().type == Symbol::Type::OPERATOR) {
                    const auto &holdingStackOp = stkHolding.front().op;
                    if (holdingStackOp.precedence >= newOp.precedence) {
                        stkOutput.push_back(stkHolding.front());
                        stkHolding.pop_front();
                    } else
                        break;
                }
            }
            stkHolding.push_front(
                {std::string(1, c), Symbol::Type::OPERATOR, newOp});
            previous = stkHolding.front();
        } else if (std::isalpha(c)) {
            size_t j = expression.find_first_of('(', i + 1);
            std::string possibleFunctionName = expression.substr(i, j - i);
            if (functions.contains(possibleFunctionName)) {
                Function function = functions[possibleFunctionName];
                stkHolding.push_front({possibleFunctionName,
                                       Symbol::Type::FUNCTION, 0, 0, false, function});
                previous = stkHolding.front();
                // One less because i++ is at the end of every iteration
                i = j - 1;
            } else
                throw std::invalid_argument("Invalid expression: '" +
                                            expression + '\'');
        } else if (c == ',') {
            while (stkHolding.front().type != Symbol::Type::PARENTHESIS_OPEN) {
                stkOutput.push_back(stkHolding.front());
                stkHolding.pop_front();
            }
            previous = COMMA_TOKEN;
        } else
            throw std::invalid_argument("Bad symbol: '" + std::string(1, c));
    }

    while (!stkHolding.empty()) {
        stkOutput.push_back(stkHolding.front());
        stkHolding.pop_front();
    }

    // for (auto &s : stkOutput) {
    //     std::cout << s.symbol << ' ';
    // }
    // std::cout << std::endl;

    return convertSymbolsToTokens(stkOutput);
}
