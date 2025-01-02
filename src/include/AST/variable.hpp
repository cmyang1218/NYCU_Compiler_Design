#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class VariableNode : public AstNode {
  private:
    // TODO: variable name, type, constant value
    std::string _name;
    std::string _type;
    ConstantValueNode *_const_val;
    VariableNode *next;

  public:
    /* TODO: variable name, type, constant value */
    VariableNode(const uint32_t line, const uint32_t col, 
                  const char *v_name);
    
    ~VariableNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::string getName();
    void setName(const char *name);
    std::string getType();
    void setType(const char *type);
    ConstantValueNode* getConstantValueNode();
    void setConstantValueNode(ConstantValueNode *constvalue);
    bool isConstantValue();
};

#endif
