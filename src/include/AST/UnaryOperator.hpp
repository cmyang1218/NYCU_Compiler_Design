#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/operator.hpp"
#include <string>

class UnaryOperatorNode : public ExpressionNode {
  private:
    // TODO: operator, expression
    std::string _operator;
    int _operator_type;
    ExpressionNode *_operand;
    ExpressionNode *next;

  public:
    /* TODO: operator, expression */
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                     int u_operator, ExpressionNode* u_operand);
                     
    ~UnaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    int getOperatorType();
    std::string getOperatorString();
    std::string getOperandType();
    
};

#endif
