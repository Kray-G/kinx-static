%pure_parser

%token ERROR
%token IF ELSE FOR WHILE FUNCTION RETURN
%token ADDEQ SUBEQ MULEQ DIVEQ MODEQ EQEQ NEQ LEQ GEQ
%token VAR NAME INT_TYPE DBL_TYPE INT_VALUE DBL_VALUE

%{
#include "lexer.h"

#define YYPARSE_PARAM parsectx
#define YYLEX_PARAM parsectx
%}

%union {
    int64_t iv;
    double dv;
}

%%

program
    : statement_list
    ;

statement_list
    : statement
    | statement_list statement
    ;

statement
    : declaration_statement
    | expression_statement
    | if_statement
    | for_statement
    | while_statement
    | return_statement
    | function_definition
    | block
    ;

block
    : '{' statement_list '}'
    ;

declaration_statement
    : var_declaration ';'
    ;

var_declaration
    : VAR declaration_list
    ;

declaration_list
    : declaration_expression
    | declaration_list ',' declaration_expression
    ;

declaration_expression
    : NAME type_info_Opt initializer_Opt
    ;

type_info_Opt
    : /* empty */
    | ':' type_name
    ;

type_name
    : INT_TYPE
    | DBL_TYPE
    ;

initializer_Opt
    : /* empty */
    | '=' expression
    ;

expression_statement
    : expression ';'
    ;

expression
    : assign_expression
    ;

assign_expression
    : compare_expression
    | assign_expression '=' compare_expression
    | assign_expression ADDEQ compare_expression
    | assign_expression SUBEQ compare_expression
    | assign_expression MULEQ compare_expression
    | assign_expression DIVEQ compare_expression
    | assign_expression MODEQ compare_expression
    ;

compare_expression
    : add_sub_expression
    | compare_expression EQEQ add_sub_expression
    | compare_expression NEQ add_sub_expression
    | compare_expression '<' add_sub_expression
    | compare_expression LEQ add_sub_expression
    | compare_expression '>' add_sub_expression
    | compare_expression GEQ add_sub_expression
    ;

add_sub_expression
    : mul_div_mod_expression
    | add_sub_expression '+' mul_div_mod_expression
    | add_sub_expression '-' mul_div_mod_expression
    ;

mul_div_mod_expression
    : postfix_expression
    | mul_div_mod_expression '*' postfix_expression
    | mul_div_mod_expression '/' postfix_expression
    | mul_div_mod_expression '%' postfix_expression
    ;

postfix_expression
    : factor
    | postfix_expression '(' call_argument_list ')'
    ;

call_argument_list
    : expression
    | call_argument_list ',' expression
    ;

factor
    : NAME
    | INT_VALUE
    | DBL_VALUE
    | '(' expression ')'
    ;

if_statement
    : IF '(' expression ')' statement else_clause_Opt
    ;

else_clause_Opt
    : /* empty */
    | ELSE statement
    ;

for_statement
    : FOR
      '(' for_expression1_Opt ';' for_expression2_Opt ';' for_expression2_Opt ')'
      statement
    ;

for_expression1_Opt
    : /* empty */
    | expression
    | var_declaration
    ;

for_expression2_Opt
    : /* empty */
    | expression
    ;

while_statement
    : WHILE '(' expression ')' statement
    ;

return_statement
    : RETURN expression ';'
    ;

function_definition
    : FUNCTION type_info_Opt NAME '(' argument_list ')' block
    ;

argument_list
    : argument
    | argument_list ',' argument
    ;

argument
    : NAME type_info_Opt
    ;

%%

int yyerror()
{
    return 0;
}
