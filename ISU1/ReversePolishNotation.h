#pragma once

#include <iosfwd>
#include <vector>

namespace ReversePolishNotation {
enum class TokenType {
    OPERATOR,
    OPERAND,
};

struct Token {
    virtual TokenType getType() = 0;
};

struct Operand : Token {
    Operand(double value);

    bool x = false, y = false;

    Operand();

    TokenType getType() override final;

    double value = 0.0;
};

static constexpr double empty(double args[], size_t amount) { return 0.0; }

struct Operator : Token {
    int numArgs = 0;  // Allow -1

    static constexpr int VARARGS = -1;

    double (*callback)(double args[], size_t amount) = empty;

    Operator();

    Operator(int numArgs, double (*callback)(double args[], size_t amount));

    Operator(const Operator &op);

    TokenType getType() override final;
};

double evaluate(const std::vector<Token *> &tokens, double x, double y);
}  // namespace ReversePolishNotation

std::ostream &operator<<(std::ostream &os,
                         const ReversePolishNotation::Operand &o);
