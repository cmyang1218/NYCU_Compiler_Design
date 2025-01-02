#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, 
                    VariableReferenceNode *r_variable_reference)
    : AstNode{line, col} {
        this->_variable_reference = r_variable_reference;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_variable_reference != nullptr) {
        this->_variable_reference->accept(p_visitor);
    }
    return;
}

AstNode* ReadNode::getNext() {
    return this->next;
}

void ReadNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

VariableReferenceNode* ReadNode::getVariableReference() {
    return this->_variable_reference;
}