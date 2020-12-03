#include "ast_node.h"

static void ast_dump_item(node_t *node)
{
    if (!node) {
        return;
    }

    switch (node->ntype) {
    case EXPR_INT: {
        break;
    }
    case EXPR_DBL: {
        break;
    }
    case EXPR_CALL: {
        break;
    }
    case EXPR_BINARY: {
        break;
    }

    case STMT_DECL: {
        break;
    }
    case STMT_EXPR: {
        break;
    }
    case STMT_IF: {
        break;
    }
    case STMT_FOR: {
        break;
    }
    case STMT_WHILE: {
        break;
    }
    case STMT_DOWHILE: {
        break;
    }
    case STMT_RET: {
        break;
    }
    case STMT_FUNC: {
        break;
    }
    default:
        ;
    }
}

void ast_dump(node_t *root)
{
    ast_dump_item(root);
}
