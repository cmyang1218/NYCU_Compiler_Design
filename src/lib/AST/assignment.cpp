#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, 
                                VariableReferenceNode *a_variable_reference,
                                ExpressionNode *a_expression)
    : AstNode{line, col} {
        this->_variable_reference = a_variable_reference;
        this->_expression = a_expression;
    }

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_variable_reference != nullptr) {
        this->_variable_reference->accept(p_visitor);
    }
    if(this->_expression != nullptr) {
        this->_expression->accept(p_visitor);
    }
    return;
}   

AstNode* AssignmentNode::getNext() {
    return this->next;
}

void AssignmentNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

VariableReferenceNode* AssignmentNode::getVariableReference() {
    return this->_variable_reference;
}

ExpressionNode* AssignmentNode::getExpression() {
    return this->_expression;
}