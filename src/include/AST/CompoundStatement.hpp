#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"
#include <vector>

class CompoundStatementNode : public AstNode {
  private:
    // TODO: declarations, statements
    std::vector<DeclNode*> _declarations;
    std::vector<AstNode*> _statements;
    CompoundStatementNode *next;
    SymbolTable *_table;
  public:
    /* TODO: declarations, statements */
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                            DeclNode *c_declarations, AstNode *c_statements);
    
    ~CompoundStatementNode() = default;
    
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void  setNext(AstNode *node) override;
    void setSymbolTable(SymbolTable *table);
    SymbolTable* getSymbolTable();
    
};

#endif
