#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class AssignmentNode : public AstNode {
  private:
    // TODO: variable reference, expression
    VariableReferenceNode *_variable_reference;
    ExpressionNode *_expression;
    AstNode *next;
    
  public:
    /* TODO: variable reference, expression */
    AssignmentNode(const uint32_t line, const uint32_t col,
                    VariableReferenceNode *a_variable_reference, 
                    ExpressionNode *a_expression);
                    
    ~AssignmentNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    VariableReferenceNode* getVariableReference();
    ExpressionNode* getExpression();
};

#endif
