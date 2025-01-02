#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, 
                            const char *f_name, DeclNode *f_declaration,
                            const char *f_return_type)
    : AstNode{line, col} {
        this->_name = f_name;
        DeclNode *decl_head = f_declaration;
        while(decl_head != nullptr) {
            this->_declarations.insert(this->_declarations.begin(), decl_head);
            decl_head = dynamic_cast<DeclNode*>(decl_head->getNext());
        }
        this->_return_type = f_return_type;
    }

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, 
                            const char *f_name, DeclNode *f_declaration,
                            const char *f_return_type, CompoundStatementNode *f_compound)
    : AstNode{line, col} {
        this->_name = f_name;
        DeclNode *decl_head = f_declaration;
        while(decl_head != nullptr) {
            this->_declarations.insert(this->_declarations.begin(), decl_head);
            decl_head = dynamic_cast<DeclNode*>(decl_head->getNext());
        }
        this->_return_type = f_return_type;
        this->_compound = f_compound;
    }

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for(auto &decl : this->_declarations) {
        decl->accept(p_visitor);
    }
    if(this->_compound != nullptr) {
        this->_compound->accept(p_visitor);
    }  
}

AstNode* FunctionNode::getNext() {
    return this->next;
}

void FunctionNode::setNext(AstNode *node) {
    this->next = dynamic_cast<FunctionNode *>(node);
    return;
}

std::string FunctionNode::getName() {
    return this->_name;
}

void FunctionNode::setName(const char *name) {
    this->_name = name;
    return;
}

std::string FunctionNode::getPrototype() {
    std::string str;
    str += (this->_return_type + " ");
    str += "(";
    bool first = true;
    for(auto &decl : this->_declarations) {
        std::vector<VariableNode*> variables = decl->getVariables();
        for(auto &var : variables) {
            std::string var_type = var->getType();
            if(first) {
                str += var_type;
                first = false;
            }else{
                str += (", " + var_type);
            }
        }
    }
    str += ")";
    return str;
}

std::string FunctionNode::getReturntype() {
    return this->_return_type;
}

std::string FunctionNode::getParamtype() {
    std::string str;
    bool first = true;
    for(auto &decl : this->_declarations) {
        std::vector<VariableNode*> variables = decl->getVariables();
        for(auto &var : variables) {
            std::string var_type = var->getType();
            if(first) {
                str += var_type;
                first = false;
            }else {
                str += (", " + var_type);
            }
        }
    }
    return str;
}

std::size_t FunctionNode::getNumberOfParams() {
    std::size_t num = 0;
    for(auto &decl : this->_declarations) {
        num += decl->getVariables().size();
    }
    return num;
}

void FunctionNode::setSymbolTable(SymbolTable *table) {
    this->_table = table;
}

SymbolTable* FunctionNode::getSymbolTable() {
    return this->_table;
}