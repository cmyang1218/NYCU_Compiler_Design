#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class FunctionInvocationNode : public ExpressionNode {
  private:
    // TODO: function name, expressions
    std::string _name;
    std::vector<ExpressionNode*> _expressions;
    AstNode *next;

  public:
    /* TODO: function name, expressions */
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                            const char *f_name, ExpressionNode *f_expressions);
    
    ~FunctionInvocationNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::string getName();
    std::size_t getNumberOfArguments();
    std::vector<std::string> getArgumentTypes();
    std::vector<Location> getArgumentLocations();
};

#endif
