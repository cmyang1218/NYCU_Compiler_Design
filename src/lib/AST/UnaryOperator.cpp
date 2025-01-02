#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, 
                                        int u_operator, ExpressionNode *u_operand)
    : ExpressionNode{line, col} {
        this->_operator = operatorStr[u_operator];
        this->_operator_type = u_operator;
        this->_operand = u_operand;
    }

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_operand != nullptr) {
        this->_operand->accept(p_visitor);
    }
    return;
}

AstNode* UnaryOperatorNode::getNext() {
    return this->next;
}

void UnaryOperatorNode::setNext(AstNode *node) {
    this->next = dynamic_cast<ExpressionNode*>(node);
    return;
}

int UnaryOperatorNode::getOperatorType() {
    return this->_operator_type;
}

std::string UnaryOperatorNode::getOperatorString() {
    return this->_operator;
}

std::string UnaryOperatorNode::getOperandType() {
    return this->_operand->getExpressionType();
}