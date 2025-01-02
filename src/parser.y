%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"

#include "AST/AstDumper.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SemanticChecker.hpp"
#include "codegen/CodeGenerator.hpp"

#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

static AstNode *root;
extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    struct ConstValue;
    class AstNode;
    class DeclNode;
    class VariableNode;
    class ConstantValueNode;
    class FunctionNode;
    class CompoundStatementNode;
    class PrintNode;
    class ExpressionNode;
    class VariableReferenceNode;
    class FunctionInvocationNode;
    class IfNode;
    class WhileNode;
    class ForNode;
    class ReturnNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    const char *const_str;
    char character;
    int integer;
    double real;
    char *string;
    bool boolean;
    AstNode *node;
    DeclNode *declaration;
    VariableNode *variable;
    ConstantValueNode *constant_value;
    FunctionNode *function;
    CompoundStatementNode *compound_statement;
    ExpressionNode *expression;
    VariableReferenceNode *variable_reference;
    FunctionInvocationNode *function_invocation;
    IfNode *if_condition;
    WhileNode *while_loop;
    ForNode *for_loop;
    ReturnNode *return_statement;
};

%type <const_str> Type ScalarType ReturnType
%type <character> NegOrNot
%type <string> ArrType ArrDecl 
%type <identifier> ProgramName FunctionName
%type <declaration> DeclarationList Declarations Declaration 
%type <declaration> FormalArgList FormalArgs FormalArg
%type <variable> IdList
%type <constant_value> LiteralConstant IntegerAndReal StringAndBoolean
%type <function> FunctionList Functions Function FunctionDeclaration FunctionDefinition
%type <compound_statement> CompoundStatement 
%type <node> StatementList Statements Statement 
%type <node> Simple
%type <expression> ExpressionList Expressions Expression
%type <variable_reference> VariableReference
%type <expression> ArrRefList ArrRefs
%type <function_invocation> FunctionInvocation FunctionCall
%type <compound_statement> ElseOrNot
%type <if_condition> Condition
%type <while_loop> While
%type <for_loop> For
%type <return_statement> Return 
    /* Follow the order in scanner.l */

    /* Delimiter */
%token <character> COMMA SEMICOLON COLON
%token <character> L_PARENTHESIS R_PARENTHESIS
%token <character> L_BRACKET R_BRACKET

    /* Operator */
%token <const_str> ASSIGN
%left  <const_str> OR
%left  <const_str> AND
%right <const_str> NOT
%left  <const_str> LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left  <const_str> PLUS MINUS
%left  <const_str> MULTIPLY DIVIDE MOD
%right <const_str> UNARY_MINUS

    /* Keyword */
%token <string> ARRAY BOOLEAN INTEGER REAL STRING
%token <string> END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token <string> DO ELSE FOR IF THEN WHILE
%token <string> DEF OF TO RETURN VAR
%token <boolean> FALSE TRUE
%token <string> PRINT READ

    /* Identifier */
%token <identifier> ID

    /* Literal */
%token <integer> INT_LITERAL
%token <real> REAL_LITERAL
%token <string> STRING_LITERAL

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1, "void", $3, $4, $5);
    }
;

ProgramName:
    ID {
        $$ = strdup($1);
    }
;

DeclarationList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Declarations {
        $$ = $1;
    }
;

Declarations:
    Declaration {
        $$ = $1;
    }
    |
    Declarations Declaration {
        $2->setNext($1);
        $$ = $2;
    }
;

FunctionList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Functions {
        $$ = $1;
    }
;

Functions:
    Function {
        $$ = $1;
    }
    |
    Functions Function {
        $2->setNext($1);
        $$ = $2;
    }
;

Function:
    FunctionDeclaration {
        $$ = $1;
    }
    |
    FunctionDefinition {
        $$ = $1;
    }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6);
    }
;

FunctionName:
    ID {
        $$ = strdup($1);
    }
;

FormalArgList:
    Epsilon {
        $$ = nullptr;
    }
    |
    FormalArgs {
        $$ = $1;
    }
;

FormalArgs:
    FormalArg {
        $$ = $1;
    }
    |
    FormalArgs SEMICOLON FormalArg {
        $3->setNext($1);
        $$ = $3;
    }
;

FormalArg:
    IdList COLON Type {
        $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3);
    }
;

IdList:
    ID {
        VariableNode *var_node = new VariableNode(@1.first_line, @1.first_column, $1);
        $$ = var_node;
    }
    |
    IdList COMMA ID {
        VariableNode *var_node = new VariableNode(@3.first_line, @3.first_column, $3);
        var_node->setNext($1);
        $$ = var_node;
    }
;

ReturnType:
    COLON ScalarType {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = "void";
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
;

Type:
    ScalarType {
        $$ = strdup($1);
    }
    |
    ArrType {
        $$ = strdup($1);
    }
;

ScalarType:
    INTEGER {
        $$ = "integer";
    }
    |
    REAL {
        $$ = "real";
    }
    |
    STRING {    
        $$ = "string";
    }
    |
    BOOLEAN {
        $$ = "boolean";
    }
;

ArrType:
    ArrDecl ScalarType {
        char *arr_type = new char[strlen($2)+strlen($1)+1];
        strcpy(arr_type, $2);
        strcat(arr_type, " ");
        strcat(arr_type, $1);
        $$ = arr_type;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        char *arr_decl = (char *)calloc(12, sizeof(char));
        sprintf(arr_decl, "[%d]", $2);
        arr_decl = (char *)realloc(arr_decl, strlen(arr_decl) * sizeof(char));
        $$ = arr_decl;
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        char *arr_decl = (char *)calloc(12, sizeof(char));
        sprintf(arr_decl, "[%d]", $3);
        arr_decl = (char *)realloc(arr_decl, strlen(arr_decl) * sizeof(char));
        int first_len = strlen($1);
        $1 = (char *)realloc($1, (first_len + strlen(arr_decl)) * sizeof(char));
        strcat($1, arr_decl);
        $$ = $1;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        ConstValue *cv = new ConstValue;
        cv->val_type = TypeEnum::TYPE_INT;
        if($1 == '+') {
            cv->int_val = $2;
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, cv);
        }else if($1 == '-') {
            cv->int_val = -1 * $2;
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
        }
    }
    |
    NegOrNot REAL_LITERAL {
        ConstValue *cv = new ConstValue;
        cv->val_type = TypeEnum::TYPE_REAL;
        if($1 == '+') {
            cv->real_val = $2;
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, cv);
        }else if ($1 == '-') {
            cv->real_val = -1 * $2;
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
        }
   }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon {
        $$ = '+';
    }
    |
    MINUS %prec UNARY_MINUS {
        $$ = '-';
    }
;

StringAndBoolean:
    STRING_LITERAL {
        ConstValue *cv = new ConstValue;
        cv->str_val = $1;
        cv->val_type = TypeEnum::TYPE_STR;
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
    }
    |
    TRUE {
        ConstValue *cv = new ConstValue;
        cv->bool_val = "true";
        cv->val_type = TypeEnum::TYPE_BOOL;
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
    }
    |
    FALSE {
        ConstValue *cv = new ConstValue;
        cv->bool_val = "false";
        cv->val_type = TypeEnum::TYPE_BOOL;
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
    }
;

IntegerAndReal:
    INT_LITERAL {
        ConstValue *cv = new ConstValue;
        cv->int_val = $1;
        cv->val_type = TypeEnum::TYPE_INT;
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
    }
    |
    REAL_LITERAL {
        ConstValue *cv = new ConstValue;
        cv->real_val = $1;
        cv->val_type = TypeEnum::TYPE_REAL;
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, cv);
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = $1;
    }
    |
    Simple {
        $$ = $1;
    }
    |
    Condition {
        $$ = $1;
    }
    |
    While {
        $$ = $1;
    }
    |
    For {
        $$ = $1;
    }
    |
    Return {
        $$ = $1;
    }
    |
    FunctionCall {
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
    }
    |
    PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList {
        if($2 != nullptr) {
            $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
        }else{
            $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1);
        }
    }
;

ArrRefList:
    Epsilon {
        $$ = nullptr;
    }
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = $2;
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $3->setNext($1);
        $$ = $3;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        if($5 != nullptr) {
            $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
        }else {
            $$ = new IfNode(@1.first_line, @1.first_column, $2, $4);
        }
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        VariableNode *var_node = new VariableNode(@2.first_line, @2.first_column, $2);
        DeclNode *decl_node = new DeclNode(@2.first_line, @2.first_column, var_node, "integer");
        
        VariableReferenceNode *var_ref_node = new VariableReferenceNode(@2.first_line, @2.first_column, $2);
        ConstValue *start_cv = new ConstValue;
        start_cv->int_val = $4;
        start_cv->val_type = TypeEnum::TYPE_INT;
        ConstantValueNode *start_const_node = new ConstantValueNode(@4.first_line, @4.first_column, start_cv);
        AssignmentNode *assign_node = new AssignmentNode(@3.first_line, @3.first_column, var_ref_node, start_const_node);
        
        ConstValue *end_cv = new ConstValue;
        end_cv->int_val = $6;
        end_cv->val_type = TypeEnum::TYPE_INT;
        ConstantValueNode *end_const_node = new ConstantValueNode(@6.first_line, @6.first_column, end_cv);
        
        $$ = new ForNode(@1.first_line, @1.first_column, decl_node, assign_node, end_const_node, $8);
    }
;

Return:
    RETURN Expression SEMICOLON {
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON {
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Expressions {
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = $1;
    }
    |
    Expressions COMMA Expression {
        $3->setNext($1);
        $$ = $3;
    }
;

StatementList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Statements {
        $$ = $1;
    }
;

Statements:
    Statement {
        $$ = $1;
    }
    |
    Statements Statement {
        $2->setNext($1);
        $$ = $2;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, OperatorEnum::OP_NEG, $2);
    }
    |
    Expression MULTIPLY Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_MULTIPLY, $1, $3);
    }
    |
    Expression DIVIDE Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_DIVIDE, $1, $3);
    }   
    |
    Expression MOD Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_MOD, $1, $3);
    }
    |
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_PLUS, $1, $3);
    }
    |
    Expression MINUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_MINUS, $1, $3);
    }
    |
    Expression LESS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_LESS, $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_LESS_EQUAL, $1, $3);
    }
    |
    Expression GREATER Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_GREATER, $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_GREATER_EQUAL, $1, $3);
    }
    |
    Expression EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_EQUAL, $1, $3);
    }
    |
    Expression NOT_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_NOT_EQUAL, $1, $3);
    }
    |
    NOT Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, OperatorEnum::OP_NOT, $2);
    }
    |
    Expression AND Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_AND, $1, $3);
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, OperatorEnum::OP_OR, $1, $3);
    }
    |
    IntegerAndReal {
        $$ = $1;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
    |
    VariableReference {
        $$ = $1;
    }
    |
    FunctionInvocation {
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);    
    }
    
    SemanticAnalyzer sema_analyzer;
    root->accept(sema_analyzer);
    std::vector<std::string> error_msgs = sema_analyzer.getSymbolManager().checker.getErrorMessages();
    std::vector<Location> error_locs = sema_analyzer.getSymbolManager().checker.getErrorLocations();
    assert(error_msgs.size() == error_locs.size());

    std::vector<std::string> src_code;
    std::ifstream in;
    in.open(argv[1]);
    while(!in.eof()) {
        std::string line;
        std::getline(in, line);
        src_code.push_back(line);
    }

    if (error_msgs.empty()) {
        printf("\n"
            "|---------------------------------------------------|\n"
            "|  There is no syntactic error and semantic error!  |\n"
            "|---------------------------------------------------|\n");
    }else{
        for(unsigned int i = 0; i < error_msgs.size(); ++i) {
            fprintf(stderr, "<Error> Found in line %d, column %d: %s", error_locs[i].line, error_locs[i].col, error_msgs[i].c_str());
            fprintf(stderr, "    %s\n", src_code[error_locs[i].line-1].c_str());
           
            std::string at = "";
            for(unsigned int j = 0; j < error_locs[i].col-1; ++j) {
                at += " ";
            }
            at += "^";
            fprintf(stderr, "    %s\n", at.c_str());
        }
    }

    CodeGenerator code_generator(argv[1], (argc == 4) ? argv[3] : "",
                                 sema_analyzer.getSymbolManager());
    root->accept(code_generator);

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
