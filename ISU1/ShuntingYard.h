#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <iosfwd>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdbool>

#include "ReversePolishNotation.h"

namespace ShuntingYard {  // olc
struct Operator {
    int precedence = 0;
    int arguments = 0;

    bool rightAssociative = false;
};

struct Function {
    int arguments;
};

struct Symbol {
    std::string symbol = "";

    enum class Type {
        UNKNOWN,
        LITERAL_NUMERIC,
        OPERATOR,
        PARENTHESIS_OPEN,
        PARENTHESIS_CLOSE,
        FUNCTION,
        COMMA,
        X,
        Y
    } type = Type::UNKNOWN;

    Operator op;

    Function function;
};

std::vector<ReversePolishNotation::Token *> convertToRPN(
    const std::string &expression);
}  // namespace ShuntingYard
