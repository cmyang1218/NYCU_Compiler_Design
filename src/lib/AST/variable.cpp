#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, 
                            const char *v_name)
    : AstNode{line, col} {
        this->_name = v_name;
    }

void VariableNode::print() {}

void VariableNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_const_val != nullptr) {
        this->_const_val->accept(p_visitor);
    }
}

AstNode* VariableNode::getNext() {
    return this->next;
}

void VariableNode::setNext(AstNode *node) {
    this->next = dynamic_cast<VariableNode*>(node);
    return;
}

std::string VariableNode::getName() {
    return this->_name;
}

void VariableNode::setName(const char *name) {
    this->_name = name;
    return;
}

std::string VariableNode::getType() {
    return this->_type;
}

void VariableNode::setType(const char *type) {
    this->_type = type;
    return;
}

ConstantValueNode* VariableNode::getConstantValueNode() {
    return this->_const_val;
}

void VariableNode::setConstantValueNode(ConstantValueNode *constvalue) {
    this->_const_val = constvalue;
    return;
}

bool VariableNode::isConstantValue() {
    ConstantValueNode* const_val = getConstantValueNode();
    if(const_val != nullptr) {
        return true;
    }
    return false;
}