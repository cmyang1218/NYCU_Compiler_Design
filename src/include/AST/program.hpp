#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"
#include <string>
#include <vector>

class ProgramNode final : public AstNode {
  private:
    std::string _name;
    // TODO: return type, declarations, functions, compound statement
    std::string _return;
    std::vector<DeclNode *> _declarations;
    std::vector<FunctionNode *> _functions;
    CompoundStatementNode *_compound;
    SymbolTable *_table;
  public:
 
    /* TODO: return type, declarations, functions, compound statement */
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *const p_name,
                const char *p_return,
                DeclNode *p_declaration,
                FunctionNode *p_function,
                CompoundStatementNode *p_compound);
    
    ~ProgramNode() = default;
    
    const char *getNameCString() const;
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    void setSymbolTable(SymbolTable *table);
    SymbolTable* getSymbolTable();
};

#endif
