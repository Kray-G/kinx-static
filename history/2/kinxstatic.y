
%token IF ELSE FOR WHILE FUNCTION
%token VAR NAME INT DBL ADDEQ SUBEQ MULEQ DIVEQ MODEQ INT_VALUE DBL_VALUE

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
    : INT
    | DBL
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
    : add_sub_expression
    | assign_expression '=' add_sub_expression
    | assign_expression ADDEQ add_sub_expression
    | assign_expression SUBEQ add_sub_expression
    | assign_expression MULEQ add_sub_expression
    | assign_expression DIVEQ add_sub_expression
    | assign_expression MODEQ add_sub_expression
    ;

add_sub_expression
    : mul_div_mod_expression
    | add_sub_expression '+' mul_div_mod_expression
    | add_sub_expression '-' mul_div_mod_expression
    ;

mul_div_mod_expression
    : factor
    | mul_div_mod_expression '*' factor
    | mul_div_mod_expression '/' factor
    | mul_div_mod_expression '%' factor
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
