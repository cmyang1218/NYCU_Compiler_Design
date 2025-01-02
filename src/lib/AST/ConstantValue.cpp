#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                        ConstValue *c_constvalue)
    : ExpressionNode{line, col} {
        this->_constvalue = c_constvalue;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

void ConstantValueNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void ConstantValueNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    return;
}

AstNode* ConstantValueNode::getNext() {
    return this->next;
}   

void ConstantValueNode::setNext(AstNode *node) {
    this->next = dynamic_cast<ExpressionNode*>(node);
    return;
}

ConstValue* ConstantValueNode::getConstantValue() {
    return this->_constvalue;
}

void ConstantValueNode::setConstantValue(ConstValue* constvalue) {
    this->_constvalue = constvalue;
    return;
}