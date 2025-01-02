#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReturnNode : public AstNode {
  private:
    // TODO: expression
    ExpressionNode *_expression;
    AstNode *next;
    
  public:
    /* TODO: expression */
    ReturnNode(const uint32_t line, const uint32_t col,
                ExpressionNode *r_expression);
    
    ~ReturnNode() = default;

    void print() override;    
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    ExpressionNode* getExpression();
};

#endif
