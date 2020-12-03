
#include "ast_node.h"

static node_manager_t *node_manager_new(void)
{
    return (node_manager_t *)calloc(1, sizeof(node_manager_t));
}

static node_t *node_new(node_manager_t *mgr)
{
    node_t *n = (node_t *)calloc(1, sizeof(node_t));
    node_manager_t *m = node_manager_new();
    m->node = n;
    m->head = mgr->head;
    mgr->head = m;
    return n;
}

void free_node_all(node_manager_t *mgr)
{
    node_manager_t *m = mgr->head;
    while (m) {
        node_manager_t *next = m->head;
        if (m->node->name) {
            string_free(m->node->name);
        }
        free(m->node);
        free(m);
        m = next;
    }
}

node_t *ast_set_root(node_manager_t *mgr, node_t *root)
{
    mgr->root = root;
    return root;
}

node_t *ast_declaration_expression(node_manager_t *mgr, string_t *name, int type, node_t *initializer)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_DECL;
    return n;
}

node_t *ast_value_int(node_manager_t *mgr, int64_t ivalue)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_INT;
    n->vtype = NODETYPE_INT;
    n->n.ivalue = ivalue;
    return n;
}

node_t *ast_value_dbl(node_manager_t *mgr, double dvalue)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_DBL;
    n->vtype = NODETYPE_DBL;
    n->n.dvalue = dvalue;
    return n;
}

node_t *ast_value_variable(node_manager_t *mgr, string_t *name)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_if_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause, node_t *else_clause)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_for_statement(node_manager_t *mgr, node_t *expr1, node_t *expr2, node_t *expr3, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_while_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_dowhile_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_return_statement(node_manager_t *mgr, node_t *expr, node_t *if_modifier)
{
    node_t *n = node_new(mgr);
    return n;
}

node_t *ast_function_statement(node_manager_t *mgr, int64_t type, string_t *name, node_t *args, node_t *block)
{
    node_t *n = node_new(mgr);
    return n;
}
