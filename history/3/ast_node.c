
#include "ast_node.h"

node_manager_t *node_manager_new(void)
{
    return (node_manager_t *)calloc(1, sizeof(node_manager_t));
}

node_t *node_new(node_manager_t *mgr, node_t *ref)
{
    node_t *n = (node_t *)calloc(1, sizeof(node_t));
    *n = *ref;
    node_manager_t *m = node_manager_new();
    m->node = n;
    m->head = mgr->head;
    mgr->head = m;
    return n;
}

void node_free_all(node_manager_t *mgr)
{
    node_manager_t *m = mgr->head;
    while (m) {
        node_manager_t *next = m->head;
        free(m);
        m = next;
    }
}
