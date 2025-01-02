#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, 
                        ExpressionNode *r_expression)
    : AstNode{line, col} {
        this->_expression = r_expression;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_expression != nullptr) {
        this->_expression->accept(p_visitor);
    }
    return;
}

AstNode* ReturnNode::getNext() {
    return this->next;
}

void ReturnNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

ExpressionNode* ReturnNode::getExpression() {
    return this->_expression;
}