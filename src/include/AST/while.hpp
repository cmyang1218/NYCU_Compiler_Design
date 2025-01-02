#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"

class WhileNode : public AstNode {
  private:
    // TODO: expression, compound statement
    ExpressionNode *_expression;
    CompoundStatementNode *_compound;
    AstNode *next;
    
  public:
    /* TODO: expression, compound statement */
    WhileNode(const uint32_t line, const uint32_t col,
              ExpressionNode *w_expression, CompoundStatementNode *w_compound);

    ~WhileNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    ExpressionNode* getExpression();
    CompoundStatementNode* getWhileStatement();
};

#endif
