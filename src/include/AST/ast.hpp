#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H
#include <cstdint>

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class ConstValue {
  public:
    int val_type; 
    int int_val;
    double real_val;
    char *str_val;
    const char *bool_val;

    ~ConstValue() = default;
    ConstValue() = default;
};

class AstNode {
  protected:
    Location location;
    
  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    const Location &getLocation() const;

    virtual void print() = 0;
    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor) = 0;
    virtual AstNode* getNext() = 0;
    virtual void setNext(AstNode *node) = 0;
};

#endif
