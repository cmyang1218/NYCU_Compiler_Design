#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/type.hpp"
#include <string>
#include <vector> 

class DeclNode : public AstNode {
  private:
    // TODO: variables
    std::vector<VariableNode*> _variables;
    DeclNode *next;

  public:
    // variable declaration
    /* TODO: identifiers, type */
    DeclNode(const uint32_t line, const uint32_t col,
              VariableNode *d_identifiers, const char *d_type);

    // constant variable declaration
    /* TODO: identifiers, constant */
    DeclNode(const uint32_t, const uint32_t col, 
              VariableNode *d_identifiers, ConstantValueNode *d_constants);

    ~DeclNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::vector<VariableNode*> getVariables();
};

#endif
