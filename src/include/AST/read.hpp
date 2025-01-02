#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReadNode : public AstNode {
  private:
    // TODO: variable reference
    VariableReferenceNode *_variable_reference;
    AstNode *next;
    
  public:
    /* TODO: variable reference */
    ReadNode(const uint32_t line, const uint32_t col,
             VariableReferenceNode *r_variable_reference);

    ~ReadNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    VariableReferenceNode* getVariableReference();
};

#endif
