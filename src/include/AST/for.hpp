#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "sema/SymbolTable.hpp"

class ForNode : public AstNode {
  private:
    // TODO: declaration, assignment, expression, compound statement
    DeclNode *_declaration;
    AssignmentNode *_assignment;
    ExpressionNode *_expression;
    CompoundStatementNode *_compound;
    AstNode *next;
    SymbolTable *_table;

  public:
    /* TODO: declaration, assignment, expression, compound statement */
    ForNode(const uint32_t line, const uint32_t col,
            DeclNode *f_declaration, AssignmentNode *f_assignment,
            ExpressionNode *f_expression, CompoundStatementNode *f_compound);

    ~ForNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::vector<VariableNode *> getVariables();
    int getLoopLowerBound();
    int getLoopUpperBound();
    void setSymbolTable(SymbolTable* table);
    SymbolTable* getSymbolTable();
    DeclNode* getDeclaration();
    AssignmentNode* getAssignment();
    ExpressionNode* getExpression();
    CompoundStatementNode* getForStatement();
};

#endif
