#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                                            const char *v_name)
    : ExpressionNode{line, col} {
        this->_name = v_name;
    }

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                                            const char *v_name, ExpressionNode *v_expression) 
    : ExpressionNode{line, col} {
        this->_name = v_name;
        ExpressionNode *expr_head = v_expression;
        while(expr_head != nullptr) {
            this->_expressions.insert(this->_expressions.begin(), expr_head);
            expr_head = dynamic_cast<ExpressionNode*>(expr_head->getNext());
        }
    }

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    return;
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for(auto &expr : this->_expressions) {
        expr->accept(p_visitor);
    }
    return;
}

AstNode* VariableReferenceNode::getNext() {
    return this->next;
}

void VariableReferenceNode::setNext(AstNode *node) {
    this->next = dynamic_cast<ExpressionNode*>(node);
    return;
}

std::string VariableReferenceNode::getName() {
    return this->_name;
}

std::size_t VariableReferenceNode::getNumberOfIndices() {
    return this->_expressions.size();
}

std::vector<std::string> VariableReferenceNode::getIndexTypes() {
    std::vector<std::string> idx_types;
    for(auto &expr : this->_expressions) {
        idx_types.push_back(expr->getExpressionType());
    }
    return idx_types;
}

std::vector<int> VariableReferenceNode::getIndexValues() {
    std::vector<int> idx_values;
    for(auto &expr : this->_expressions) {
        ConstantValueNode *con = dynamic_cast<ConstantValueNode *>(expr);
        idx_values.push_back(con->getConstantValue()->int_val);
    }
    return idx_values;
}

std::vector<Location> VariableReferenceNode::getIndexLocations() {
    std::vector<Location> idx_locs; 
    for(auto &expr : this->_expressions) {
        idx_locs.push_back(expr->getLocation());
    }
    return idx_locs;
}


void VariableReferenceNode::setVariableKind(std::string kind) {
    this->_kind = kind;
    return;
}

std::string VariableReferenceNode::getVariableKind() {
    return this->_kind;
}