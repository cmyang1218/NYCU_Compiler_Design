#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, 
                ExpressionNode *i_expression, CompoundStatementNode *i_if_compound)
    : AstNode{line, col} {
        this->_expression = i_expression;
        this->_if_compound = i_if_compound;
    }
    
IfNode::IfNode(const uint32_t line, const uint32_t col, 
                ExpressionNode *i_expression, CompoundStatementNode *i_if_compound, 
                CompoundStatementNode *i_else_compound)
    : AstNode{line, col} {
        this->_expression = i_expression;
        this->_if_compound = i_if_compound;
        this->_else_compound = i_else_compound;
    }

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_expression != nullptr) {
        this->_expression->accept(p_visitor);
    }
    if(this->_if_compound != nullptr) {
        this->_if_compound->accept(p_visitor);
    }
    if(this->_else_compound != nullptr) {
        this->_else_compound->accept(p_visitor);
    }
    return;
}

AstNode* IfNode::getNext() {
    return this->next;
}

void IfNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

ExpressionNode* IfNode::getExpression() {
    return this->_expression;
}


CompoundStatementNode* IfNode::getIfStatement() {
    return this->_if_compound;
}

CompoundStatementNode* IfNode::getElseStatement() {
    return this->_else_compound;
}