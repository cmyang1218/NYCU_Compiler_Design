#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class PrintNode : public AstNode {
  private:
    // TODO: expression
    ExpressionNode *_expression;
    AstNode *next;

  public:
    PrintNode(const uint32_t line, const uint32_t col,
              ExpressionNode *p_expression);
    
    ~PrintNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::string getExpressionType();
    Location getExpressionLocation();
};

#endif
