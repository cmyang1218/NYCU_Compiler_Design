#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, 
                                                DeclNode *c_declarations, 
                                                AstNode *c_statements)
    : AstNode{line, col} {
        DeclNode *decl_head = c_declarations;
        while(decl_head != nullptr) {
            this->_declarations.insert(this->_declarations.begin(), decl_head);
            decl_head = dynamic_cast<DeclNode*>(decl_head->getNext());
        }
        AstNode *stat_head = c_statements;
        while(stat_head != nullptr) {
            this->_statements.insert(this->_statements.begin(), stat_head);
            stat_head = stat_head->getNext();
        }
    }

void CompoundStatementNode::print() {}

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for(auto &decl : this->_declarations) {
        decl->accept(p_visitor);
    }
    
    for(auto &stat : this->_statements) {
        stat->accept(p_visitor);
    }
    return;
}

AstNode* CompoundStatementNode::getNext() { 
    return this->next;
}

void CompoundStatementNode::setNext(AstNode *node) {
    this->next = dynamic_cast<CompoundStatementNode*>(node);
    return;
}

void CompoundStatementNode::setSymbolTable(SymbolTable *table) {
    this->_table = table;
}

SymbolTable* CompoundStatementNode::getSymbolTable() {
    return this->_table;
}
