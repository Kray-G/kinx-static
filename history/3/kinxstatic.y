%pure_parser

%{
#include "lexer.h"
#include "ast_node.h"

#define YYPARSE_PARAM parsectx
#define YYLEX_PARAM parsectx

#define NODEMGR (((kxs_parsectx_t *)parsectx)->node_mgr)
%}

%union {
    int64_t iv;
    double dv;
    string_t *sv;
    node_t *node;
}

%token ERROR
%token IF ELSE FOR DO WHILE FUNCTION RETURN
%token ADDEQ SUBEQ MULEQ DIVEQ MODEQ EQEQ NEQ LEQ GEQ
%token<iv> INT_TYPE DBL_TYPE
%token<iv> INT_VALUE
%token<dv> DBL_VALUE
%token<sv> VAR NAME

%type<node> program
%type<node> statement_list
%type<node> statement
%type<node> block
%type<node> declaration_statement
%type<node> var_declaration
%type<node> declaration_list
%type<node> declaration_expression
%type<iv> type_info_Opt
%type<iv> type_name
%type<node> initializer_Opt
%type<node> expression_statement
%type<node> expression
%type<node> assign_expression
%type<node> compare_expression
%type<node> add_sub_expression
%type<node> mul_div_mod_expression
%type<node> postfix_expression
%type<node> call_argument_list
%type<node> factor
%type<node> if_statement
%type<node> else_clause_Opt
%type<node> for_statement
%type<node> for_expression1_Opt
%type<node> for_expression2_Opt
%type<node> while_statement
%type<node> do_while_statement
%type<node> return_statement
%type<node> if_modifier_Opt
%type<node> function_definition
%type<node> argument_list
%type<node> argument

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
    | do_while_statement
    | return_statement
    | function_definition
    | block
    ;

block
    : '{' statement_list '}' { $$ = $2; }
    ;

declaration_statement
    : var_declaration ';'
    ;

var_declaration
    : VAR declaration_list { $$ = $2; }
    ;

declaration_list
    : declaration_expression
    | declaration_list ',' declaration_expression
    ;

declaration_expression
    : NAME type_info_Opt initializer_Opt { $$ = ast_declaration_expression(NODEMGR, $1, $2, $3); }
    ;

type_info_Opt
    : /* empty */ { $$ = NODETYPE_INT; }
    | ':' type_name { $$ = $2; }
    ;

type_name
    : INT_TYPE { $$ = NODETYPE_INT; }
    | DBL_TYPE { $$ = NODETYPE_DBL; }
    ;

initializer_Opt
    : /* empty */ { $$ = NULL; }
    | '=' expression { $$ = $2; }
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
    : NAME { $$ = ast_value_variable(NODEMGR, $1); }
    | INT_VALUE { $$ = ast_value_int(NODEMGR, $1); }
    | DBL_VALUE { $$ = ast_value_dbl(NODEMGR, $1); }
    | '(' expression ')' { $$ = $2; }
    ;

if_statement
    : IF '(' expression ')' statement else_clause_Opt { $$ = ast_if_statement(NODEMGR, $3, $5, $6); }
    ;

else_clause_Opt
    : /* empty */ { $$= NULL; }
    | ELSE statement { $$ = $2; }
    ;

for_statement
    : FOR
      '(' for_expression1_Opt ';' for_expression2_Opt ';' for_expression2_Opt ')'
      statement
        { $$ = ast_for_statement(NODEMGR, $3, $5, $7, $9); }
    ;

for_expression1_Opt
    : /* empty */ { $$ = NULL; }
    | expression
    | var_declaration
    ;

for_expression2_Opt
    : /* empty */ { $$ = NULL; }
    | expression
    ;

while_statement
    : WHILE '(' expression ')' statement { $$ = ast_while_statement(NODEMGR, $3, $5); }
    ;

do_while_statement
    : DO statement WHILE '(' expression ')' ';' { $$ = ast_dowhile_statement(NODEMGR, $5, $2); }
    ;

return_statement
    : RETURN expression if_modifier_Opt ';' { $$ = ast_return_statement(NODEMGR, $2, $3); }
    ;

if_modifier_Opt
    : /* empty */
    | IF '(' expression ')' { $$ = $3; }
    ;

function_definition
    : FUNCTION type_info_Opt NAME '(' argument_list ')' block
        { $$ = ast_function_statement(NODEMGR, $2, $3, $5, $7); }
    ;

argument_list
    : argument
    | argument_list ',' argument
    ;

argument
    : NAME type_info_Opt { $$ = ast_declaration_expression(NODEMGR, $1, $2, NULL); }
    ;

%%

int yyerror(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    return 0;
}
