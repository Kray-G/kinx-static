#ifndef AST_NODE_H
#define AST_NODE_H

#include <stdint.h>

enum node_type {
    NODE_UNKNOWN,

    EXPR_INT,
    EXPR_DBL,
    EXPR_CALL,
    EXPR_BINARY,

    STMT_DECL,
    STMT_EXPR,
    STMT_IF,
    STMT_FOR,
    STMT_WHILE,
    STMT_DOWHILE,
    STMT_RET,
    STMT_FUNC,
};

enum value_type {
    NODETYPE_UNKNOWN,
    NODETYPE_INT,
    NODETYPE_DBL,
};

enum expr_type {
    EXPRTYPE_UNKNOWN,
    EXPRTYPE_INT,
    EXPRTYPE_DBL,
};

struct node_t_;
typedef struct node_manager_t_ {
    struct node_t_ *node;
    struct node_manager_t_ *head;
} node_manager_t;

typedef struct node_t_ {
    node_manager_t *mgr;            // reference to an allocation manager.
    struct node_t_ *next;           // for statement or expression list.
    struct node_t_ *last;           // for statement or expression list.

    enum node_type ntype;
    enum value_type vtype;          // type of node. if ntype == STMT_FUNC, this means a return type.
    union {
        int64_t ivalue;             // EXPR_INT
        double dvalue;              // EXPR_DBL
        union {
            struct expr_binary {    // EXPR_BINARY
                enum expr_type etype;
                struct node_t_ *lhs;
                struct node_t_ *rhs;
            } binary;
            struct expr_call {     // EXPR_CALL
                enum expr_type etype;
                struct node_t_ *func;
                struct node_t_ *args;
            } call;
        } e;
        union {
            struct stmt_decl {      // STMT_DECL
                struct node_t_ *expr;
            } decl;
            struct stmt_if {        // STMT_IF
                struct node_t_ *expr;
                struct node_t_ *then_cloause;
                struct node_t_ *else_cloause;
            } ifs;
            struct stmt_for {       // STMT_FOR
                struct node_t_ *e1;
                struct node_t_ *e2;
                struct node_t_ *e3;
                struct node_t_ *then_cloause;
            } fors;
            struct stmt_while {     // STMT_WHILE
                struct node_t_ *e1;
                struct node_t_ *then_cloause;
            } whiles;
            struct stmt_dowhile {   // STMT_DOWHILE
                struct node_t_ *e1;
                struct node_t_ *then_cloause;
            } dowhile;
            struct stmt_ret {       // STMT_RET
                struct node_t_ *expr;
            } ret;
            struct stmt_func {      // STMT_FUNC
                struct node_t_ *args;
                struct node_t_ *block;
            } func;
        } s;
    } n;
} node_t;

#endif /* AST_NODE_H */
