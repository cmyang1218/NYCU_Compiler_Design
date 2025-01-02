#ifndef CODEGEN_CODE_GENERATOR_H
#define CODEGEN_CODE_GENERATOR_H

#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <vector>
#include <map>

class StackPos{
  public:
    int _pos = -12;
    StackPos() = default;
};

class CodeGenerator final : public AstNodeVisitor {
  private:
    SymbolManager _manager;
    std::string _path;
    FILE* _file;
    bool _declConst = false;
    bool _getLvalue = false;
    bool _refStr = false;
    bool _refReal = false;
    int _labelcnt = 1;
    std::vector<StackPos> _stackpos;
    bool _isIfCond = false;
    bool _isWhileCond = false;
    std::string _funcName = "";
    std::map<std::string, char *> _strInfo;
    std::map<std::string, double> _realInfo;
  public:
    ~CodeGenerator() = default;
    CodeGenerator(std::string source_file_name,
                  std::string save_path,
                  SymbolManager p_symbol_manager);

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;
    void BeginFunc(const char *func_name);
    void EndFunc(const char *func_name);
};

#endif
