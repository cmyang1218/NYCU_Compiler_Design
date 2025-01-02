#include "AST/program.hpp"



ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, 
                         const char *p_return,
                         DeclNode *p_declaration,
                         FunctionNode *p_function,
                         CompoundStatementNode *p_compound)
    : AstNode{line, col}, _name(p_name) {
        this->_return = p_return;
        DeclNode *decl_head = p_declaration;
        while(decl_head != nullptr) {
            this->_declarations.insert(this->_declarations.begin(), decl_head);
            decl_head = dynamic_cast<DeclNode*>(decl_head->getNext());
        }
        FunctionNode *func_head = p_function;
        while(func_head != nullptr) {
            this->_functions.insert(this->_functions.begin(), func_head);
            func_head = dynamic_cast<FunctionNode*>(func_head->getNext());
        }
        this->_compound = p_compound;
    }

const char *ProgramNode::getNameCString() const {
    return this->_name.c_str(); 
}

void ProgramNode::print() {}

void ProgramNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for(auto &decl : this->_declarations) {
        decl->accept(p_visitor);
    }
    for(auto &func : this->_functions) {
        func->accept(p_visitor);
    }
    this->_compound->accept(p_visitor);
    return;
}

AstNode* ProgramNode::getNext() { return nullptr; }

void ProgramNode::setNext(AstNode *node) {}

void ProgramNode::setSymbolTable(SymbolTable *table) {
    this->_table = table;
    return;
}

SymbolTable* ProgramNode::getSymbolTable() {
    return this->_table;
}