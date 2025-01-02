#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class ConstantValueNode : public ExpressionNode {
  private:
    // TODO: constant value
    ConstValue *_constvalue;
    ExpressionNode *next;
    
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      ConstValue *c_constvalue);

    ~ConstantValueNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    ConstValue* getConstantValue();
    void setConstantValue(ConstValue *constvalue);
};

#endif
