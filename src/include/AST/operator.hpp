#ifndef __AST_OPERATOR_H
#define __AST_OPERATOR_H

enum OperatorEnum : int {
    OP_NEG,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MOD,
    OP_PLUS,
    OP_MINUS,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_NOT,
    OP_AND,
    OP_OR
};
extern const char *operatorStr[];

#endif