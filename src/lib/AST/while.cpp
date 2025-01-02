#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                    ExpressionNode* w_expression, CompoundStatementNode *w_compound)
    : AstNode{line, col} {
        this->_expression = w_expression;
        this->_compound = w_compound;
    }

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_expression != nullptr) {
        this->_expression->accept(p_visitor);
    }
    if(this->_compound != nullptr) {
        this->_compound->accept(p_visitor);
    }
    return;
}

AstNode* WhileNode::getNext() {
    return this->next;
}

void WhileNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

ExpressionNode* WhileNode::getExpression() {
    return this->_expression;
}

CompoundStatementNode* WhileNode::getWhileStatement() {
    return this->_compound;
}