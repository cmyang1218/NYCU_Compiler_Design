#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
                    VariableNode *d_identifiers, const char *d_type)
    : AstNode{line, col} {
        VariableNode *identifiers_head = d_identifiers;
        while(identifiers_head != nullptr) {
            identifiers_head->setType(d_type);
            this->_variables.insert(this->_variables.begin(), identifiers_head);
            identifiers_head = dynamic_cast<VariableNode*>(identifiers_head->getNext());
        }
    }

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
                    VariableNode *d_identifiers, ConstantValueNode *d_constants)
   : AstNode{line, col} {
        VariableNode *identifiers_head = d_identifiers;
        
        while(identifiers_head != nullptr) {
            Location loc = identifiers_head->getLocation();
            identifiers_head->setConstantValueNode(d_constants);
            identifiers_head->setType(typeStr[d_constants->getConstantValue()->val_type]);
            this->_variables.insert(this->_variables.begin(), identifiers_head);
            identifiers_head = dynamic_cast<VariableNode*>(identifiers_head->getNext());
        }
   }

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    for(auto &variables : this->_variables){
        variables->accept(p_visitor);
    }
}

AstNode* DeclNode::getNext() { 
    return this->next;
}

void DeclNode::setNext(AstNode *node) {
    this->next = dynamic_cast<DeclNode *>(node);
}

std::vector<VariableNode*> DeclNode::getVariables() {
    return this->_variables;
}