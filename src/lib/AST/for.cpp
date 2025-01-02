#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                DeclNode *f_declaration, AssignmentNode *f_assignment,
                ExpressionNode *f_expression, CompoundStatementNode *f_compound)
    : AstNode{line, col} {
        this->_declaration = f_declaration;
        this->_assignment = f_assignment;
        this->_expression = f_expression;
        this->_compound = f_compound;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(this->_declaration != nullptr) {
        this->_declaration->accept(p_visitor);
    }
    if(this->_assignment != nullptr) {
        this->_assignment->accept(p_visitor);
    }
    if(this->_expression != nullptr) {
        this->_expression->accept(p_visitor);
    }
    if(this->_compound != nullptr) {
        this->_compound->accept(p_visitor);
    }
    return;
}

AstNode* ForNode::getNext() {
    return this->next;
}

void ForNode::setNext(AstNode *node) {
    this->next = node;
}

std::vector<VariableNode *> ForNode::getVariables() {
    return this->_declaration->getVariables();
}

int ForNode::getLoopLowerBound() {
    ConstantValueNode* const_val = static_cast<ConstantValueNode *>(this->_assignment->getExpression());
    return const_val->getConstantValue()->int_val;
}

int ForNode::getLoopUpperBound() {
    ConstantValueNode* const_val = static_cast<ConstantValueNode *>(this->_expression);
    return const_val->getConstantValue()->int_val;
}

void ForNode::setSymbolTable(SymbolTable *table) {
    this->_table = table;
    return;
}

SymbolTable* ForNode::getSymbolTable() {
    return this->_table;
}

DeclNode* ForNode::getDeclaration() {
    return this->_declaration;
}

AssignmentNode* ForNode::getAssignment() {
    return this->_assignment;
}

ExpressionNode* ForNode::getExpression() {
    return this->_expression;
}

CompoundStatementNode* ForNode::getForStatement() {
    return this->_compound;
}