#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class ExpressionNode : public AstNode {
  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
    std::string _exprType;
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;
    std::string getExpressionType();
    void setExpressionType(std::string exprType);
};

#endif
