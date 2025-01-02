#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"
#include "sema/SymbolTable.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::BeginFunc(const char *func_name) {
    this->_stackpos.push_back({});
    this->_funcName = func_name;
    dumpInstructions(this->_file, ".section    .text\n");
    dumpInstructions(this->_file, "    .align 2\n");
    dumpInstructions(this->_file, "    .globl %s\n", func_name);
    dumpInstructions(this->_file, "    .type %s, @function\n", func_name);
    dumpInstructions(this->_file, "%s:\n", func_name);
    dumpInstructions(this->_file, "    addi sp, sp, -128\n");
    dumpInstructions(this->_file, "    sw ra, 124(sp)\n");
    dumpInstructions(this->_file, "    sw s0, 120(sp)\n");
    dumpInstructions(this->_file, "    addi s0, sp, 128\n");
    return;
}

void CodeGenerator::EndFunc(const char *func_name) {
    dumpInstructions(this->_file, "    lw ra, 124(sp)\n");
    dumpInstructions(this->_file, "    lw s0, 120(sp)\n");
    dumpInstructions(this->_file, "    addi sp, sp, 128\n");
    dumpInstructions(this->_file, "    jr ra\n");
    dumpInstructions(this->_file, "    .size %s, .-%s\n", func_name, func_name);
    this->_funcName = "";
    this->_stackpos.pop_back();
    return;
}


CodeGenerator::CodeGenerator(std::string source_file_name,
                             std::string save_path,
                             SymbolManager p_symbol_manager)
    : _manager(p_symbol_manager), _path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    std::string real_path =
        (save_path == "") ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    std::string output_file_path(
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
    this->_file = fopen(output_file_path.c_str(), "w");
    assert(this->_file != NULL && "Failed to open file\n");
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    this->_manager.reconstructHashTableFromSymbolTable(
                    p_program.getSymbolTable());
    dumpInstructions(this->_file, "    .file \"%s\"\n", this->_path.c_str());
    dumpInstructions(this->_file, "    .option nopic\n");
    // global variables and constants
    SymbolTable* table = p_program.getSymbolTable();
    std::vector<SymbolEntry> variables, constants;
    for(auto &entry : table->getEntries()) {
        if(entry.getKind() == "constant") { 
            constants.push_back(entry);
        }else if(entry.getKind() == "variable") {
            variables.push_back(entry);
        }
    }
    if(!variables.empty()) {
        for(auto &var : variables) {
            dumpInstructions(this->_file, ".comm %s, 4, 4\n", var.getName().c_str());
        }
    }
    if(!constants.empty()) {
        dumpInstructions(this->_file, ".section    .rodata\n");
        dumpInstructions(this->_file, "    .align 2\n");
        for(auto &con : constants) {
            dumpInstructions(this->_file, "    .globl %s\n", con.getName().c_str());
            dumpInstructions(this->_file, "    .type %s, @object\n", con.getName().c_str());
            dumpInstructions(this->_file, "%s:\n", con.getName().c_str());
            dumpInstructions(this->_file, "    .word %s\n", con.getAttribute().c_str());
        }
    }
    p_program.visitChildNodes(*this);
    for(auto &str : this->_strInfo) {
        dumpInstructions(this->_file, ".section    .rodata\n");
        dumpInstructions(this->_file, "    .align 2\n");
        dumpInstructions(this->_file, "%s:\n", str.first.c_str());
        dumpInstructions(this->_file, "    .string \"%s\"\n", str.second);
    }
    for(auto &real : this->_realInfo) {
        dumpInstructions(this->_file, ".section    .rodata\n");
        dumpInstructions(this->_file, "    .align 2\n");
        dumpInstructions(this->_file, "%s:\n", real.first.c_str());
        dumpInstructions(this->_file, "    .float %f\n", real.second); 
    }
    this->_manager.removeSymbolsFromHashTable(
                    p_program.getSymbolTable());
    return;
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
    if(this->_declConst) {
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    sw t0, 0(t1)\n");
        this->_declConst = false;
    }
    return;
}

void CodeGenerator::visit(VariableNode &p_variable) {
    SymbolEntry *entry = this->_manager.lookupFromHashTable(p_variable.getName());

    if(entry->getKind() == "constant" && entry->getLevel() > 0) {
        dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos());
        dumpInstructions(this->_file, "    addi sp, sp, -4\n");
        dumpInstructions(this->_file, "    sw t0, 0(sp)\n");    
        this->_declConst = true;
    }
    p_variable.visitChildNodes(*this);
    return;
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    p_constant_value.visitChildNodes(*this);
    
    if(this->_funcName != "") {
        ConstValue *const_val = p_constant_value.getConstantValue();
        switch(const_val->val_type) {
            case TypeEnum::TYPE_INT:
                dumpInstructions(this->_file, "    li t0, %d\n", const_val->int_val);
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
                break;
            case TypeEnum::TYPE_BOOL:
                dumpInstructions(this->_file, "    li t0, %d\n", (strcmp(const_val->bool_val, "true") == 0) ? 1 : 0);
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
                break;
            case TypeEnum::TYPE_REAL:
            case TypeEnum::TYPE_STR:
            default:
                break;
        }
    }
    return;
}

void CodeGenerator::visit(FunctionNode &p_function) {
    this->_manager.reconstructHashTableFromSymbolTable(
                    p_function.getSymbolTable());
    this->BeginFunc(p_function.getName().c_str());
    
    p_function.visitChildNodes(*this);
    
    this->EndFunc(p_function.getName().c_str());
    this->_manager.removeSymbolsFromHashTable(
                    p_function.getSymbolTable());
    return;
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    this->_manager.reconstructHashTableFromSymbolTable(
                    p_compound_statement.getSymbolTable());
    bool mainFunc = false;
    if(this->_funcName == "") {
        this->BeginFunc("main");
        mainFunc = true;
    }
    SymbolTable *table = p_compound_statement.getSymbolTable();
    std::vector<SymbolEntry> entries = table->getEntries();
    StackPos &pos = this->_stackpos.back();
    std::size_t cnt = 0;
    
    for(auto &entry : entries) {
        entry.setLocalPos(pos._pos);
        if(entry.getKind() == "parameter") {
            if(entry.getType() == "real") {
                if(cnt >= 8) {
                    dumpInstructions(this->_file, "    fsw ft%d, %d(s0)\n", cnt-5, entry.getLocalPos());
                }else {
                    dumpInstructions(this->_file, "    fsw fa%d, %d(s0)\n", cnt, entry.getLocalPos());
                }
                this->_refReal = true;
            }else if(entry.getType() == "string") {
                if(cnt >= 8) {
                    dumpInstructions(this->_file, "    sw t%d, %d(s0)\n", cnt-5, entry.getLocalPos());
                }else {
                    dumpInstructions(this->_file, "    sw a%d, %d(s0)\n", cnt, entry.getLocalPos());
                }
                this->_refStr = true;
            }else {
                if(cnt >= 8) {
                    dumpInstructions(this->_file, "    sw t%d, %d(s0)\n", cnt-5, entry.getLocalPos());
                }else {
                    dumpInstructions(this->_file, "    sw a%d, %d(s0)\n", cnt, entry.getLocalPos());
                }                
            }
            cnt++;
        }
        std::vector<int> indices = this->_manager.getArrayIndices(entry.getType());
        if(indices.size() != 0) {
            int width = 1;
            for(auto &index : indices) {
                width *= index;
            }
            pos._pos -= 4 * width;
        }else {
            pos._pos -= 4;  
        }
        
    }
    p_compound_statement.visitChildNodes(*this);
    if(mainFunc) {
        this->EndFunc("main");
    }
    this->_manager.removeSymbolsFromHashTable(
                    p_compound_statement.getSymbolTable());
    return;
}

void CodeGenerator::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    if(this->_refStr) {
        dumpInstructions(this->_file, "    lw a0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");  
        dumpInstructions(this->_file, "    jal ra, printString\n");
    }else if (this->_refReal) {
        dumpInstructions(this->_file, "    flw fa0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    jal ra, printReal\n");
    }else {
        dumpInstructions(this->_file, "    lw a0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    jal ra, printInt\n");
    }
    return;
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    bool ifCond = this->_isIfCond;
    bool whileCond = this->_isWhileCond;
    this->_isIfCond = false;
    this->_isWhileCond = false;
    p_bin_op.visitChildNodes(*this);
    this->_isIfCond = ifCond;
    this->_isWhileCond = whileCond;
    int bin_op = p_bin_op.getOperatorType();
    if(this->_refReal) {
        dumpInstructions(this->_file, "    flw ft0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    flw ft1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    }else {
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    }
    switch(bin_op) {
        case OperatorEnum::OP_PLUS:
            if(this->_refReal) {
                dumpInstructions(this->_file, "    fadd.s ft0, ft1, ft0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n");                        
            }else {
                dumpInstructions(this->_file, "    add t0, t1, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");      
            }
            break; 
        case OperatorEnum::OP_MINUS:
            if(this->_refReal) {
                dumpInstructions(this->_file, "    fsub.s ft0, ft1, ft0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n");
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_MULTIPLY:
            if(this->_refReal) {
                dumpInstructions(this->_file, "    fmul.s ft0, ft1, ft0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n");               
            }else {
                dumpInstructions(this->_file, "    mul t0, t1, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_DIVIDE:
            if(this->_refReal) {
                dumpInstructions(this->_file, "    fdiv.s ft0, ft1, ft0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n");
            }else {
                dumpInstructions(this->_file, "    div t0, t1, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_MOD:
            dumpInstructions(this->_file, "    rem t0, t1, t0\n");
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            break;
        case OperatorEnum::OP_AND:
            dumpInstructions(this->_file, "    and t0, t1, t0\n");
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            break;
        case OperatorEnum::OP_OR:
            dumpInstructions(this->_file, "    or t0, t1, t0\n");
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            break;
        case OperatorEnum::OP_EQUAL:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    bne t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    seqz t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_NOT_EQUAL:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    beq t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    snez t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_GREATER:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    ble t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    sgtz t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_GREATER_EQUAL:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    blt t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    sgtz t0, t0\n");
                dumpInstructions(this->_file, "    seqz t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        case OperatorEnum::OP_LESS:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    bge t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    sltz t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            
            break;
        case OperatorEnum::OP_LESS_EQUAL:
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    bgt t1, t0, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    sub t0, t1, t0\n");
                dumpInstructions(this->_file, "    sltz t0, t0\n");
                dumpInstructions(this->_file, "    seqz t0, t0\n");
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            }
            break;
        default: 
            break;
    }

    return;
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    bool ifCond = this->_isIfCond;
    bool whileCond = this->_isWhileCond;
    this->_isIfCond = false;
    this->_isWhileCond = false;
    p_un_op.visitChildNodes(*this);
    this->_isIfCond = ifCond;
    this->_isWhileCond = whileCond;
    int un_op = p_un_op.getOperatorType();
    dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
    dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    switch(un_op) {
        case OperatorEnum::OP_NEG:
            dumpInstructions(this->_file, "    sub t0, zero, t0\n");  
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
            break;
        case OperatorEnum::OP_NOT:
            dumpInstructions(this->_file, "    xor t0, t0, -1\n");
            dumpInstructions(this->_file, "    addi t0, t0, 2\n");
            if(this->_isIfCond || this->_isWhileCond) {
                dumpInstructions(this->_file, "    beq t0, zero, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");                
            }
            break;
        default:
            break;
    }
    return;
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    bool ifCond = this->_isIfCond;
    bool whileCond = this->_isWhileCond;
    this->_isIfCond = false;
    this->_isWhileCond = false;
    p_func_invocation.visitChildNodes(*this);
    this->_isIfCond = ifCond;
    this->_isWhileCond = whileCond;
    std::size_t num_arg = p_func_invocation.getNumberOfArguments();
    if(this->_refReal) {
        for(int i = num_arg-1; i >= 0; --i) {
            if(i >= 8) {
                dumpInstructions(this->_file, "    flw ft%d, 0(sp)\n", i-5);
            }else {
                dumpInstructions(this->_file, "    flw fa%d, 0(sp)\n", i);
            }
            dumpInstructions(this->_file, "    addi sp, sp, 4\n"); 
        }       
    }else {
        for(int i = num_arg-1; i >= 0; --i) {
            if(i >= 8) {
                dumpInstructions(this->_file, "    lw t%d, 0(sp)\n", i-5);
            }else {
                dumpInstructions(this->_file, "    lw a%d, 0(sp)\n", i);
            }
            dumpInstructions(this->_file, "    addi sp, sp, 4\n"); 
        }
    }
    dumpInstructions(this->_file, "    jal ra, %s\n", p_func_invocation.getName().c_str());
    SymbolEntry *entry = this->_manager.lookupFromHashTable(p_func_invocation.getName());
    if(entry != nullptr && entry->getType() != "void") {
        if(this->_refReal) {
            dumpInstructions(this->_file, "    fmv.s ft0, fa0\n");
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n"); 
        }else {
            dumpInstructions(this->_file, "    mv t0, a0\n");
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n"); 
        }
    }
       
    if(this->_isIfCond || this->_isWhileCond) {
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    beq t0, zero, L%d\n", this->_labelcnt+1);
    }
    return;
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    bool ifCond = this->_isIfCond;
    bool whileCond = this->_isWhileCond;
    this->_isIfCond = false;
    this->_isWhileCond = false;
    if(p_variable_ref.getNumberOfIndices() == 0) {
        p_variable_ref.visitChildNodes(*this);        
    }
    this->_isIfCond = ifCond;
    this->_isWhileCond = whileCond;
    SymbolEntry *entry = this->_manager.lookupFromHashTable(p_variable_ref.getName());
    if(entry != nullptr) {
        if(entry->getLevel() == 0) {
            dumpInstructions(this->_file, "    la t0, %s\n", entry->getName().c_str());
            if (!this->_getLvalue) { 
                dumpInstructions(this->_file, "    lw t1, 0(t0)\n");
                dumpInstructions(this->_file, "    mv t0, t1\n");
            }  
            if(this->_isIfCond || this->_isWhileCond) {
                if(entry != nullptr && entry->getType() == "boolean")
                    dumpInstructions(this->_file, "    beq t0, zero, L%d\n", this->_labelcnt+1);
            }else {
                dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                dumpInstructions(this->_file, "    sw t0, 0(sp)\n");         
            }    
        }else {
            if(strncmp(entry->getType().c_str(), "integer", 7) == 0 || strncmp(entry->getType().c_str(), "boolean", 7) == 0) {
                if(p_variable_ref.getNumberOfIndices() > 0) {
                    int offset = 0;
                    std::vector<int> arr_max_size = this->_manager.getArrayIndices(entry->getType());
                    std::vector<int> arr_act_size = p_variable_ref.getIndexValues();
                    assert(arr_max_size.size() == arr_act_size.size());
                    for(std::size_t i = 0; i < arr_max_size.size(); ++i) {
                        if(i == 0) {
                            offset += arr_act_size[i];
                        }else {
                            offset *= arr_max_size[i-1];
                            offset += arr_act_size[i];
                        }
                    }
                    if(this->_funcName == "main") {
                        dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos() - 4 * offset);
                    }else {
                        dumpInstructions(this->_file, "    addi t0, a0, %d\n", -4 * offset);
                    }
                }else {
                    if(this->_manager.getArrayIndices(entry->getType()).size() > 0){
                        this->_getLvalue = true;
                    }    
                    dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos());    
                }
                
                if (!this->_getLvalue) { 
                    dumpInstructions(this->_file, "    lw t1, 0(t0)\n");
                    dumpInstructions(this->_file, "    mv t0, t1\n");
                }    
                if(this->_isIfCond || this->_isWhileCond) {
                    if(entry != nullptr && entry->getType() == "boolean")
                        dumpInstructions(this->_file, "    beq t0, zero, L%d\n", this->_labelcnt+1);
                }else {
                    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                    dumpInstructions(this->_file, "    sw t0, 0(sp)\n");        
                }       
            }
            else if(entry->getType() == "string" && !this->_getLvalue) {
                if(this->_funcName != "main") {
                    dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos());
                    dumpInstructions(this->_file, "    lw t1, 0(t0)\n");
                    dumpInstructions(this->_file, "    mv t0, t1\n");
                    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                    dumpInstructions(this->_file, "    sw t0, 0(sp)\n"); 
                }else {
                    dumpInstructions(this->_file, "    lui t0, %chi(%s)\n", '%', entry->getName().c_str());
                    dumpInstructions(this->_file, "    addi t0, t0, %clo(%s)\n", '%', entry->getName().c_str());
                    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                    dumpInstructions(this->_file, "    sw t0, 0(sp)\n");                       
                }
                this->_refStr = true;     
            }else if(entry->getType() == "real") {
                if(this->_funcName != "main") {
                    dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos());
                    if(!this->_getLvalue) {
                        dumpInstructions(this->_file, "    lw t1, 0(t0)\n");
                        dumpInstructions(this->_file, "    mv t0, t1\n");
                    }
                    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                    dumpInstructions(this->_file, "    sw t0, 0(sp)\n"); 
                }else if(!this->_getLvalue){
                    dumpInstructions(this->_file, "    lui t0, %chi(%s)\n", '%', entry->getName().c_str());
                    dumpInstructions(this->_file, "    flw ft0, %clo(%s)(t0)\n", '%', entry->getName().c_str());
                    dumpInstructions(this->_file, "    fsw ft0, %d(s0)\n", entry->getLocalPos());
                    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
                    dumpInstructions(this->_file, "    fsw ft0, 0(sp)\n");                       
                }
                this->_refReal = true;      
            }
        }
    }else {
        dumpInstructions(this->_file, "    la t0, %s\n", p_variable_ref.getName().c_str());  
        if (!this->_getLvalue) { 
            dumpInstructions(this->_file, "    lw t1, 0(t0)\n");
            dumpInstructions(this->_file, "    mv t0, t1\n");
        }   
        if(this->_isIfCond || this->_isWhileCond) {
            if(entry != nullptr && entry->getType() == "boolean")
                dumpInstructions(this->_file, "    beq t0, zero, L%d\n", this->_labelcnt+1);
        }else {
            dumpInstructions(this->_file, "    addi sp, sp, -4\n");
            dumpInstructions(this->_file, "    sw t0, 0(sp)\n");         
        }
    }
    this->_getLvalue = false;
    return;
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    VariableReferenceNode* var_ref = p_assignment.getVariableReference();
    SymbolEntry *entry = this->_manager.lookupFromHashTable(var_ref->getName());
    if(entry != nullptr) {
        ConstantValueNode *con = dynamic_cast<ConstantValueNode *>(p_assignment.getExpression());
        if(con != nullptr) {
            if(entry->getType() == "string") {
                this->_strInfo[entry->getName()] = con->getConstantValue()->str_val;
            }else if(entry->getType() == "real") {
                this->_realInfo[entry->getName()] = con->getConstantValue()->real_val;
            }
        }
    }
    this->_getLvalue = true;
    p_assignment.visitChildNodes(*this);
    if((!this->_refStr && !this->_refReal) || this->_funcName != "main") {
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    sw t0, 0(t1)\n");
    }

    return;
}

void CodeGenerator::visit(ReadNode &p_read) {
    this->_getLvalue = true;
    p_read.visitChildNodes(*this);
    dumpInstructions(this->_file, "    jal ra, readInt\n");
    dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
    dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    dumpInstructions(this->_file, "    sw a0, 0(t0)\n");
    return;
}

void CodeGenerator::visit(IfNode &p_if) {
    ExpressionNode* expr = p_if.getExpression();
    this->_isIfCond = true;
    if(expr != nullptr) {
        expr->accept(*this);
    }
    this->_isIfCond = false;
    CompoundStatementNode* if_compound = p_if.getIfStatement();
    if(if_compound != nullptr) {
        dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
        if_compound->accept(*this);
        
    }
    CompoundStatementNode* else_compound = p_if.getElseStatement();
    dumpInstructions(this->_file, "    j L%d\n", (else_compound == nullptr) ? this->_labelcnt : this->_labelcnt+1);
    if(else_compound != nullptr) {
        dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
        else_compound->accept(*this);
    }
    dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
    return;
}

void CodeGenerator::visit(WhileNode &p_while) {
    int loop_label = this->_labelcnt;
    
    dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
    ExpressionNode* expr = p_while.getExpression();
    this->_isWhileCond = true;
    if(expr != nullptr) {
        expr->accept(*this);
    }
    this->_isWhileCond = false;
    CompoundStatementNode* while_compound = p_while.getWhileStatement();
    if(while_compound != nullptr) {
        dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
        while_compound->accept(*this);
        dumpInstructions(this->_file, "    j L%d\n", loop_label);
    }
    dumpInstructions(this->_file, "L%d:\n", this->_labelcnt++);
    return;
}

void CodeGenerator::visit(ForNode &p_for) {
    this->_manager.reconstructHashTableFromSymbolTable(
        p_for.getSymbolTable());
    int forLabel = this->_labelcnt++;
    int endLabel = this->_labelcnt++;
    SymbolTable* table = p_for.getSymbolTable();
    std::vector<SymbolEntry> entries = table->getEntries();
    StackPos &pos = this->_stackpos.back();
    for(auto &entry : entries) {
        entry.setLocalPos(pos._pos);
        pos._pos -= 4;  
    }
    DeclNode* decl = p_for.getDeclaration();
    if(decl != nullptr) {
        decl->accept(*this);
    }
    AssignmentNode* assign = p_for.getAssignment();
    if(assign != nullptr) {
        assign->accept(*this);
    }

    dumpInstructions(this->_file, "L%d:\n", forLabel);
    
    SymbolEntry* entry = this->_manager.lookupFromHashTable(assign->getVariableReference()->getName());
    dumpInstructions(this->_file, "    lw t0, %d(s0)\n", entry->getLocalPos());
    dumpInstructions(this->_file, "    addi sp, sp, -4\n");
    dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
    

    ExpressionNode* expr = p_for.getExpression();
    if(expr != nullptr) {
        expr->accept(*this);
    }
    dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
    dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
    dumpInstructions(this->_file, "    addi sp, sp, 4\n");
    dumpInstructions(this->_file, "    bge t1, t0, L%d\n", endLabel);
    
    CompoundStatementNode* for_compound = p_for.getForStatement();
    if(for_compound != nullptr) {
        for_compound->accept(*this);
        dumpInstructions(this->_file, "    addi t0, s0, %d\n", entry->getLocalPos());
        dumpInstructions(this->_file, "    addi sp, sp, -4\n");
        dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    lw t0, %d(s0)\n", entry->getLocalPos());
        dumpInstructions(this->_file, "    addi sp, sp, -4\n");
        dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    li t0, 1\n");
        dumpInstructions(this->_file, "    addi sp, sp, -4\n");
        dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    add t0, t1, t0\n");
        dumpInstructions(this->_file, "    addi sp, sp, -4\n");
        dumpInstructions(this->_file, "    sw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    lw t1, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    sw t0, 0(t1)\n");
        dumpInstructions(this->_file, "    j L%d\n", forLabel);
    }
    dumpInstructions(this->_file, "L%d:\n", endLabel);
    this->_manager.removeSymbolsFromHashTable(
        p_for.getSymbolTable());
    return;
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    if(this->_refReal) {
        dumpInstructions(this->_file, "    flw ft0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    fmv.s fa0, ft0\n");
    }else {
        dumpInstructions(this->_file, "    lw t0, 0(sp)\n");
        dumpInstructions(this->_file, "    addi sp, sp, 4\n");
        dumpInstructions(this->_file, "    mv a0, t0\n");
    }
    return;
}