#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"

class IfNode : public AstNode {
  private:
    // TODO: expression, compound statement, compound statement
    ExpressionNode *_expression;
    CompoundStatementNode *_if_compound;
    CompoundStatementNode *_else_compound;
    AstNode *next;

  public:
    /* TODO: expression, compound statement, compound statement */
    IfNode(const uint32_t line, const uint32_t col, 
          ExpressionNode *i_expression, CompoundStatementNode *i_if_compound);
    
    IfNode(const uint32_t line, const uint32_t col, 
          ExpressionNode *i_expression, CompoundStatementNode *i_if_compound, 
          CompoundStatementNode *i_else_compound);

    ~IfNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    ExpressionNode* getExpression();
    CompoundStatementNode* getIfStatement();
    CompoundStatementNode* getElseStatement();
};

#endif
