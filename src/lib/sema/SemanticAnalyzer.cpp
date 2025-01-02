#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

SymbolManager SemanticAnalyzer::getSymbolManager() {
    return this->manager;
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO: 
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    SymbolTable* table = new SymbolTable(TableTypeEnum::TABLE_PROGRAM);
    this->manager.pushScope(table);
    this->manager.insertEntryToTable({
        p_program.getNameCString(),
        "program", 
        this->manager.getLevel(),
        "void",
        "",
        p_program.getLocation(),
        false
    });
    this->manager.pushProcedure("void");
    p_program.visitChildNodes(*this);
    p_program.setSymbolTable(table);
    this->manager.popProcedure();
    this->manager.popScope();
    return;
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    
    p_decl.visitChildNodes(*this);
    return;
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    SymbolTable* curr_table = this->manager.getCurrentTable();

    if (curr_table->getTableType() == TableTypeEnum::TABLE_FUNCTION) {
        this->manager.insertEntryToTable({
            p_variable.getName().c_str(),
            "parameter", 
            this->manager.getLevel(), 
            p_variable.getType().c_str(), 
            "",
            p_variable.getLocation(),
            false
        });
        return;
    }else if (curr_table->getTableType() == TableTypeEnum::TABLE_FORLOOP) {
        return;
    }

    if(p_variable.isConstantValue()) {
        ConstValue* const_val = p_variable.getConstantValueNode()->getConstantValue();
        switch (const_val->val_type) {
            case TypeEnum::TYPE_INT:
                this->manager.insertEntryToTable({
                    p_variable.getName().c_str(), 
                    "constant", 
                    this->manager.getLevel(), 
                    p_variable.getType().c_str(), 
                    std::to_string(const_val->int_val).c_str(),
                    p_variable.getLocation(),
                    false
                });
                break;
            case TypeEnum::TYPE_REAL:
                this->manager.insertEntryToTable({
                    p_variable.getName().c_str(), 
                    "constant", 
                    this->manager.getLevel(), 
                    p_variable.getType().c_str(), 
                    std::to_string(const_val->real_val).c_str(),
                    p_variable.getLocation(),
                    false
                });
                break;
            case TypeEnum::TYPE_STR:
                this->manager.insertEntryToTable({
                    p_variable.getName().c_str(), 
                    "constant", 
                    this->manager.getLevel(), 
                    p_variable.getType().c_str(), 
                    const_val->str_val,
                    p_variable.getLocation(),
                    false
                });
                break;
            case TypeEnum::TYPE_BOOL:
                this->manager.insertEntryToTable({
                    p_variable.getName().c_str(), 
                    "constant", 
                    this->manager.getLevel(), 
                    p_variable.getType().c_str(), 
                    const_val->bool_val,
                    p_variable.getLocation(),
                    false
                });
                break;
            default: 
                break;
        }
    }else {
        std::vector<int> indices = this->manager.getArrayIndices(p_variable.getType());
        bool dimension_error = false;
        for(auto &index : indices) {
            if(index == 0) {
                dimension_error = true;
                break;
            }
        }
        if(dimension_error) {
            Location error_loc = p_variable.getLocation();
            std::string error_msg = "'" + p_variable.getName() + "' declared as an array with an index that is not greater than 0\n";
            this->manager.checker.appendErrorMessages(error_msg);
            this->manager.checker.appendErrorLocations(error_loc);
        }   
        this->manager.insertEntryToTable({
            p_variable.getName().c_str(), 
            "variable", 
            this->manager.getLevel(), 
            p_variable.getType().c_str(), 
            "",
            p_variable.getLocation(),
            dimension_error
        });
    }
    p_variable.visitChildNodes(*this);
    return;
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    ConstValue* const_val = p_constant_value.getConstantValue();
    p_constant_value.setExpressionType(typeStr[const_val->val_type]);
    return;
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    this->manager.insertEntryToTable({
        p_function.getName().c_str(),
        "function",   
        this->manager.getLevel(),
        p_function.getReturntype().c_str(),
        p_function.getParamtype().c_str(),
        p_function.getLocation(),
        false
    });
       
    SymbolTable* table = new SymbolTable(TableTypeEnum::TABLE_FUNCTION);
    this->manager.pushScope(table);
    this->manager.pushProcedure(p_function.getReturntype());
    p_function.visitChildNodes(*this);
    p_function.setSymbolTable(table);
    this->manager.popProcedure();
    this->manager.popScope();
    return;
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolTable* curr_table = this->manager.getCurrentTable();
    if(curr_table->getTableType() == TableTypeEnum::TABLE_FUNCTION) {
        curr_table->setTableType(TableTypeEnum::TABLE_COMPOUND);
        p_compound_statement.visitChildNodes(*this);
        p_compound_statement.setSymbolTable(curr_table);
    }else{
        SymbolTable* table = new SymbolTable(TableTypeEnum::TABLE_COMPOUND);
        this->manager.pushScope(table);
        p_compound_statement.visitChildNodes(*this);
        p_compound_statement.setSymbolTable(table);
        this->manager.popScope();
    }
    return;
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);
    std::string expr_type = p_print.getExpressionType();
    if(expr_type == "unknown") {
        return;
    }
    bool nonscalartype_error = (expr_type != "integer") && (expr_type != "real") && (expr_type != "string") && (expr_type != "boolean");
    if(nonscalartype_error) {
        Location error_loc = p_print.getExpressionLocation();
        std::string error_msg = "expression of print statement must be scalar type\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    } 
    return;
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_bin_op.visitChildNodes(*this);
    int bin_op = p_bin_op.getOperatorType();
    std::string l_type = p_bin_op.getLeftOperandType();
    std::string r_type = p_bin_op.getRightOperandType();
    if (l_type == "unknown" || r_type == "unknown") {
        p_bin_op.setExpressionType("unknown");
        return;
    }
    bool invalidoperand_error = false;
    switch (bin_op) {
        case OperatorEnum::OP_PLUS:
            if(l_type == "string" && r_type == "string") {
                p_bin_op.setExpressionType("string");
            }else if(l_type == "integer" && r_type == "integer") {
                p_bin_op.setExpressionType("integer");
            }else if(l_type == "real" && r_type == "integer") {
                p_bin_op.setExpressionType("real");
            }else if(l_type == "integer" && r_type == "real") {
                p_bin_op.setExpressionType("real");
            }else if (l_type == "real" && r_type == "real") {
                p_bin_op.setExpressionType("real");
            }else {
                invalidoperand_error = true;
            }  
            break;
        case OperatorEnum::OP_MULTIPLY:
        case OperatorEnum::OP_DIVIDE:
        case OperatorEnum::OP_MINUS:
            if(l_type == "integer" && r_type == "integer") {
                p_bin_op.setExpressionType("integer");
            }else if(l_type == "real" && r_type == "integer") {
                p_bin_op.setExpressionType("real");
            }else if(l_type == "integer" && r_type == "real") {
                p_bin_op.setExpressionType("real");
            }else if (l_type == "real" && r_type == "real") {
                p_bin_op.setExpressionType("real");
            }else {
                invalidoperand_error = true;
            }
            break;
        case OperatorEnum::OP_MOD:
            if(l_type == "integer" && r_type == "integer") {
                p_bin_op.setExpressionType("integer");
            }else {
                invalidoperand_error = true;
            }
            break;
        case OperatorEnum::OP_LESS:
        case OperatorEnum::OP_LESS_EQUAL:
        case OperatorEnum::OP_GREATER:
        case OperatorEnum::OP_GREATER_EQUAL:
        case OperatorEnum::OP_EQUAL:
        case OperatorEnum::OP_NOT_EQUAL:
            if(l_type == "integer" && r_type == "integer") {
                p_bin_op.setExpressionType("boolean");
            }else if(l_type == "real" && r_type == "integer") {
                p_bin_op.setExpressionType("boolean");
            }else if(l_type == "integer" && r_type == "real") {
                p_bin_op.setExpressionType("boolean");
            }else if(l_type == "real" && r_type == "real") {
                p_bin_op.setExpressionType("boolean");
            }else {
                invalidoperand_error = true;
            }
            break;
        case OperatorEnum::OP_AND:
        case OperatorEnum::OP_OR:
            if(l_type == "boolean" && r_type == "boolean") {
                p_bin_op.setExpressionType("boolean");
            }else {
                invalidoperand_error = true;
            }
            break;
        default:
            invalidoperand_error = true;
            break;
    }
   
    if(invalidoperand_error) {
        p_bin_op.setExpressionType("unknown");
        Location error_loc = p_bin_op.getLocation();
        std::string error_msg = "invalid operands to binary operator '" + p_bin_op.getOperatorString() + "' ('"+ l_type + "' and '" + r_type + "')\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_un_op.visitChildNodes(*this);
    int un_op = p_un_op.getOperatorType();
    std::string opnd_type = p_un_op.getOperandType();
    if(opnd_type == "unknown") {
        p_un_op.setExpressionType("unknown");
        return;
    }
    bool invalidoperand_error = false;
    switch (un_op) {
        case OperatorEnum::OP_NEG:
            if(opnd_type == "integer") {
                p_un_op.setExpressionType("integer");
            }else if(opnd_type == "real") {
                p_un_op.setExpressionType("real");
            }else {
                invalidoperand_error = true;
            }
            break;
        case OperatorEnum::OP_NOT:
            if(opnd_type == "boolean") {
                p_un_op.setExpressionType("boolean");
            }else {
                invalidoperand_error = true;
            }
            break;
        default:  
            break;
    }
    if(invalidoperand_error) {
        p_un_op.setExpressionType("unknown");
        Location error_loc = p_un_op.getLocation();
        std::string error_msg = "invalid operand to unary operator '" + p_un_op.getOperatorString() + "' ('"+ opnd_type + "')\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_func_invocation.visitChildNodes(*this);
    bool undeclared_error = true, nonfunction_error = false;
    bool argumentnumber_error = false, argumenttype_error = false;
    std::string func_invoc_name = p_func_invocation.getName();
    std::vector<SymbolTable *> iter_tables = this->manager.getIterableTables();
    std::string func_invoc_type;
    std::string func_invoc_attr;
    std::vector<std::string> func_params;
    for(auto &table : iter_tables) {
        std::vector<SymbolEntry> iter_entries = table->getEntries();
        for(auto &entry : iter_entries) {
            if(func_invoc_name == entry.getName()) {
                undeclared_error = false;
                if (entry.getKind() != "function") {
                    nonfunction_error = true;
                }
                func_invoc_type = entry.getType();
                func_invoc_attr = entry.getAttribute();
                break;
            }
        }
    }
    if(undeclared_error) {
        p_func_invocation.setExpressionType("unknown");
        Location error_loc = p_func_invocation.getLocation();
        std::string error_msg = "use of undeclared symbol '" + func_invoc_name + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else if(nonfunction_error) {
        p_func_invocation.setExpressionType("unknown");
        Location error_loc = p_func_invocation.getLocation();
        std::string error_msg = "call of non-function symbol '" + func_invoc_name + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else {
        p_func_invocation.setExpressionType(func_invoc_type);
        std::size_t func_args = p_func_invocation.getNumberOfArguments();
        std::size_t strpos = func_invoc_attr.find(", ");
        while(strpos != std::string::npos) {
            std::string s = func_invoc_attr.substr(0, strpos);
            func_params.push_back(s);
            func_invoc_attr = func_invoc_attr.substr(strpos+2);
            strpos = func_invoc_attr.find(", ");
        }
        if(!func_invoc_attr.empty()) {
            func_params.push_back(func_invoc_attr);
        }
        argumentnumber_error = (func_args != func_params.size());
        if(argumentnumber_error) {
            p_func_invocation.setExpressionType("unknown");
            Location error_loc = p_func_invocation.getLocation();
            std::string error_msg = "too few/much arguments provided for function '" + func_invoc_name + "'\n";
            this->manager.checker.appendErrorLocations(error_loc);
            this->manager.checker.appendErrorMessages(error_msg);
        }else {
            std::vector<std::string> func_arg_types = p_func_invocation.getArgumentTypes();
            std::vector<Location> func_arg_locs = p_func_invocation.getArgumentLocations();
            Location arg_loc = p_func_invocation.getLocation();
            std::string arg_type, param_type;
            for(std::size_t i = 0; i < func_params.size(); ++i) {
                if(func_arg_types[i] != func_params[i] || func_arg_types[i] == "unknown") {
                    argumenttype_error = true;
                    arg_type = func_arg_types[i];
                    param_type = func_params[i];
                    arg_loc = func_arg_locs[i];
                    break;
                }
            }
            if (arg_type == "unknown") {
                p_func_invocation.setExpressionType("unknown");
                return;
            }
            if(argumenttype_error) {
                p_func_invocation.setExpressionType("unknown");
                Location error_loc = arg_loc;
                std::string error_msg = "incompatible type passing '" + arg_type + "' to parameter of type '" + param_type + "'\n";
                this->manager.checker.appendErrorLocations(error_loc);
                this->manager.checker.appendErrorMessages(error_msg);
            }
        }
    }
    return;
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_variable_ref.visitChildNodes(*this);
    bool undeclared_error = true, nonvariable_error = false;
    bool indextype_error = false, oversubscript_error = false;
    bool check_entry_error = false;
    std::string var_ref_name = p_variable_ref.getName();
    std::vector<SymbolTable *> iter_tables = this->manager.getIterableTables();
    std::vector<int> decl_indices;
    std::string var_ref_type;
    for(auto &table : iter_tables) {
        std::vector<SymbolEntry> iter_entries = table->getEntries();
        for(auto &entry : iter_entries) {
            if(var_ref_name == entry.getName()) {
                undeclared_error = false;
                if(entry.getKind() != "parameter" && entry.getKind() != "variable" && entry.getKind() != "loop_var" && entry.getKind() != "constant") {
                    nonvariable_error = true;
                }
                p_variable_ref.setVariableKind(entry.getKind());
                check_entry_error = entry.getStatus();
                var_ref_type = entry.getType();
                break;
            }   
        } 
    }
    if(check_entry_error) {
        p_variable_ref.setExpressionType("unknown");
    }else if(undeclared_error) {
        p_variable_ref.setExpressionType("unknown");
        Location error_loc = p_variable_ref.getLocation();
        std::string error_msg = "use of undeclared symbol '" + var_ref_name + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else if(nonvariable_error) {
        p_variable_ref.setExpressionType("unknown");
        Location error_loc = p_variable_ref.getLocation();
        std::string error_msg = "use of non-variable symbol '" + var_ref_name + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else {
        decl_indices = this->manager.getArrayIndices(var_ref_type);
        std::size_t var_ref_indices = p_variable_ref.getNumberOfIndices();
        std::size_t primitive_type_pos = var_ref_type.find(" ");
        std::string indices_str = "";
        if (primitive_type_pos != std::string::npos) {
            indices_str += var_ref_type.substr(0, primitive_type_pos);
            if(var_ref_indices < decl_indices.size()) {
                indices_str += " ";
                for(std::size_t i = var_ref_indices; i < decl_indices.size(); ++i) {
                    indices_str += "[" + std::to_string(decl_indices[i]) + "]";
                }
            }
            p_variable_ref.setExpressionType(indices_str); 
        }else {
            p_variable_ref.setExpressionType(var_ref_type);
        }
        oversubscript_error = (var_ref_indices > decl_indices.size());

        if(oversubscript_error) {
            p_variable_ref.setExpressionType("unknown");
            Location error_loc = p_variable_ref.getLocation();
            std::string error_msg = "there is an over array subscript on '" + p_variable_ref.getName() + "'\n";
            this->manager.checker.appendErrorLocations(error_loc);
            this->manager.checker.appendErrorMessages(error_msg);
        }else {
            std::vector<std::string> idx_types = p_variable_ref.getIndexTypes();
            std::vector<Location> idx_locs = p_variable_ref.getIndexLocations();
            Location idx_loc = p_variable_ref.getLocation();
            std::string idx_type;
            for(std::size_t i = 0; i < idx_types.size(); ++i) {
                if(idx_types[i] != "integer") {
                    indextype_error = true;
                    idx_type = idx_types[i];
                    idx_loc = idx_locs[i];
                    break;
                }
            }
            if(idx_type == "unknown") {
                p_variable_ref.setExpressionType("unknown");
                return;
            }
            if(indextype_error) {
                p_variable_ref.setExpressionType("unknown");
                Location error_loc = idx_loc;
                std::string error_msg = "index of array reference must be an integer\n";
                this->manager.checker.appendErrorLocations(error_loc);
                this->manager.checker.appendErrorMessages(error_msg);
            }
        }
    }
    return;
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_assignment.visitChildNodes(*this);
    VariableReferenceNode* var_ref = p_assignment.getVariableReference();
    std::string var_ref_name = var_ref->getName();
    std::string var_ref_type = var_ref->getExpressionType();
    std::string var_ref_kind = var_ref->getVariableKind();
    
    ExpressionNode* expr = p_assignment.getExpression();
    std::string expr_type = expr->getExpressionType();
    if(var_ref_type == "unknown") {
        return;
    }
    SymbolTable* curr_table = this->manager.getCurrentTable();
    if(curr_table->getTableType() == TableTypeEnum::TABLE_FORLOOP) {
        return;
    }
    
    std::vector<int> is_varref_arr = this->manager.getArrayIndices(var_ref_type);
    std::vector<int> is_expr_arr = this->manager.getArrayIndices(expr_type);
    bool varref_arr_error = (is_varref_arr.size() != 0);
    bool varref_const_error = (var_ref_kind == "constant");
    bool varref_loopvar_error = (var_ref_kind == "loop_var");
    bool expr_arr_error = (is_expr_arr.size() != 0);
    bool varref_expr_type_error = true;
    if(var_ref_type == expr_type) {
        varref_expr_type_error = false;
    }else if (var_ref_type == "integer" && expr_type == "real") {
        varref_expr_type_error = false;
    }else if (var_ref_type == "real" && expr_type == "integer") {
        varref_expr_type_error = false;
    }
    if(varref_arr_error) {
        Location error_loc = var_ref->getLocation();
        std::string error_msg = "array assignment is not allowed\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
        return;
    }else if(varref_const_error) {
        Location error_loc = var_ref->getLocation();
        std::string error_msg = "cannot assign to variable '" + var_ref_name + "' which is a constant\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
        return;
    }else if(varref_loopvar_error) {
        Location error_loc = var_ref->getLocation();
        std::string error_msg = "the value of loop variable cannot be modified inside the loop body\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
        return;
    }
    if(expr_type == "unknown") {
        return;
    }
    if(expr_arr_error) {
        Location error_loc = expr->getLocation();
        std::string error_msg = "array assignment is not allowed\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else if(varref_expr_type_error) {
        Location error_loc = p_assignment.getLocation();
        std::string error_msg = "assigning to '" + var_ref_type + "' from incompatible type '" + expr_type + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_read.visitChildNodes(*this);
    VariableReferenceNode* var_ref = p_read.getVariableReference();
    std::string var_ref_type = var_ref->getExpressionType();
    std::string var_ref_kind = var_ref->getVariableKind();
    if(var_ref_type == "unknown") {
        return;
    }
    bool nonscalartype_error = (var_ref_type != "integer") && (var_ref_type != "real") && (var_ref_type != "string") && (var_ref_type != "boolean");
    bool const_loopvar_error = (var_ref_kind == "constant") || (var_ref_kind == "loop_var");
    if(nonscalartype_error) {
        Location error_loc = var_ref->getLocation();
        std::string error_msg = "variable reference of read statement must be scalar type\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }else if (const_loopvar_error) {
        Location error_loc = var_ref->getLocation();
        std::string error_msg = "variable reference of read statement cannot be a constant or loop variable\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);
    ExpressionNode* expr = p_if.getExpression();
    std::string expr_type = expr->getExpressionType();
    if(expr_type == "unknown") {
        return;
    }
    if(expr_type != "boolean") {
        Location error_loc = expr->getLocation();
        std::string error_msg = "the expression of condition must be boolean type\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);
    ExpressionNode* expr = p_while.getExpression();
    std::string expr_type = expr->getExpressionType();
    if(expr_type == "unknown") {
        return;
    }
    if(expr_type != "boolean") {
        Location error_loc = expr->getLocation();
        std::string error_msg = "the expression of condition must be boolean type\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    return;
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    
    int lower = p_for.getLoopLowerBound(), upper = p_for.getLoopUpperBound();
    bool increment_error = (lower > upper);
    if(increment_error) {
        Location error_loc = p_for.getLocation();
        std::string error_msg = "the lower bound and upper bound of iteration count must be in the incremental order\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
    }
    SymbolTable *table = new SymbolTable(TableTypeEnum::TABLE_FORLOOP);
    this->manager.pushScope(table);
    
    VariableNode* var = p_for.getVariables()[0];
    
    this->manager.insertEntryToTable({
        var->getName().c_str(),
        "loop_var",
        this->manager.getLevel(), 
        var->getType().c_str(),
        "",
        var->getLocation(),
        increment_error
    });
    this->manager.lockLoopVariables(var->getName());
    p_for.visitChildNodes(*this);
    p_for.setSymbolTable(table);
    this->manager.unlockLoopVariables();
    this->manager.popScope();
    return;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_return.visitChildNodes(*this);
    ExpressionNode* expr = p_return.getExpression();
    std::string expr_type = expr->getExpressionType();
    std::string proc_type = this->manager.getCurrentProcTable();

    bool ret_void_error = (proc_type == "void");
    bool ret_unmatch_error = (expr_type != proc_type);
    if(ret_void_error) {
        Location error_loc = p_return.getLocation();
        std::string error_msg = "program/procedure should not return a value\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
        return;
    }
    if(expr_type == "unknown") {
        return;
    }
    if (ret_unmatch_error) {
        Location error_loc = expr->getLocation();
        std::string error_msg = "return '" + expr_type + "' from a function with return type '" + proc_type + "'\n";
        this->manager.checker.appendErrorLocations(error_loc);
        this->manager.checker.appendErrorMessages(error_msg);
        return;
    }
    return;
}
