#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, 
                        ExpressionNode *p_expression)
    : AstNode{line, col} {
        this->_expression = p_expression;
    }

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    this->_expression->accept(p_visitor);
    return;
}

AstNode* PrintNode::getNext() {
    return this->next;
} 

void PrintNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

std::string PrintNode::getExpressionType() {
    return this->_expression->getExpressionType();
}

Location PrintNode::getExpressionLocation() {
    return this->_expression->getLocation();
}