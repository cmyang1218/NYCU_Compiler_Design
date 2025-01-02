#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col,
                                            const char *f_name, ExpressionNode *f_expressions)
    : ExpressionNode{line, col} {
        this->_name = f_name;
        ExpressionNode *expr_head = f_expressions;
        while(expr_head != nullptr) {
            this->_expressions.insert(this->_expressions.begin(), expr_head);
            expr_head = dynamic_cast<ExpressionNode*>(expr_head->getNext());
        }
    }

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for(auto &expr : this->_expressions) {
        expr->accept(p_visitor);
    }
    return;
}

AstNode* FunctionInvocationNode::getNext() {
    return this->next;
}

void FunctionInvocationNode::setNext(AstNode *node) {
    this->next = node;
    return;
}

std::string FunctionInvocationNode::getName() {
    return this->_name;
}

std::size_t FunctionInvocationNode::getNumberOfArguments() {
    return this->_expressions.size();
}

std::vector<std::string> FunctionInvocationNode::getArgumentTypes() {
    std::vector<std::string> arg_types;
    for(auto &expr : this->_expressions) {
        arg_types.push_back(expr->getExpressionType());
    }
    return arg_types;
}

std::vector<Location> FunctionInvocationNode::getArgumentLocations() {
    std::vector<Location> arg_locs;
    for(auto &expr : this->_expressions) {
        arg_locs.push_back(expr->getLocation());
    }
    return arg_locs;
}