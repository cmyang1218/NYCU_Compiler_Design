#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class VariableReferenceNode : public ExpressionNode {
  private:
    // TODO: variable name, expressions
    std::string _name;
    std::string _kind;
    std::vector<ExpressionNode*> _expressions;
    ExpressionNode *next;
    
  public:
    // normal reference
    /* TODO: name */
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *v_name);
    // array reference
    /* TODO: name, expressions */
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *v_name, ExpressionNode *v_expression);
    
    ~VariableReferenceNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::string getName();
    std::size_t getNumberOfIndices();
    std::vector<std::string> getIndexTypes();
    std::vector<int> getIndexValues();
    std::vector<Location> getIndexLocations();
    void setVariableKind(std::string kind);
    std::string getVariableKind();
};

#endif
