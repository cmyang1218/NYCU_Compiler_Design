#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/operator.hpp"
#include <string>

class BinaryOperatorNode : public ExpressionNode {
  private:
    // TODO: operator, expressions
    std::string _operator;
    int _operator_type;
    ExpressionNode *_left_operand;
    ExpressionNode *_right_operand;
    ExpressionNode *next;
    
  public:
    /* TODO: operator, expressions */
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       int b_operator, ExpressionNode *b_left_operand,
                       ExpressionNode *b_right_operand);

    ~BinaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    int getOperatorType();
    std::string getOperatorString();
    std::string getLeftOperandType();
    std::string getRightOperandType();
};

#endif
