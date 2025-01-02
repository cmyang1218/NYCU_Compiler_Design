#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "sema/SymbolTable.hpp"
#include <vector>
#include <string>

class FunctionNode : public AstNode {
  private:
    // TODO: name, declarations, return type, compound statement
    std::string _name;
    std::vector<DeclNode *> _declarations;
    std::string _return_type;
    CompoundStatementNode *_compound;
    FunctionNode *next;
    SymbolTable *_table;

  public:
    /* TODO: name, declarations, return type, compound statement (optional) */
    FunctionNode(const uint32_t line, const uint32_t col, 
                  const char *f_name, DeclNode *f_declaration,
                  const char *f_return_type);
    
    FunctionNode(const uint32_t line, const uint32_t col, 
                  const char *f_name, DeclNode *f_declaration,
                  const char *f_return_type, CompoundStatementNode *f_compound);
    
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    AstNode* getNext() override;
    void setNext(AstNode *node) override;
    std::string getName();
    void setName(const char *name);
    std::string getPrototype();
    std::string getReturntype();
    std::string getParamtype();
    std::size_t getNumberOfParams();
    void setSymbolTable(SymbolTable *table);
    SymbolTable* getSymbolTable();

};

#endif
