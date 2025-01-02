#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, 
                                        int b_operator, ExpressionNode *b_left_operand, 
                                        ExpressionNode *b_right_operand)
    : ExpressionNode{line, col} {
        this->_operator = operatorStr[b_operator];
        this->_operator_type = b_operator;
        this->_left_operand = b_left_operand;
        this->_right_operand = b_right_operand;
    }

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}
void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_left_operand != nullptr) {
        this->_left_operand->accept(p_visitor);
    }
    if(this->_right_operand != nullptr) {
        this->_right_operand->accept(p_visitor);
    }
    return;
}

AstNode* BinaryOperatorNode::getNext() {
    return this->next;
}

void BinaryOperatorNode::setNext(AstNode *node) {
    this->next = dynamic_cast<ExpressionNode*>(node);
    return;
}

int BinaryOperatorNode::getOperatorType() {
    return this->_operator_type;
}

std::string BinaryOperatorNode::getOperatorString() {
    return this->_operator;
}

std::string BinaryOperatorNode::getLeftOperandType() {
    return this->_left_operand->getExpressionType();
}

std::string BinaryOperatorNode::getRightOperandType() {
    return this->_right_operand->getExpressionType();
} 