#include "lexer.h"

extern int yyparse(kxs_parsectx_t *);
extern int yydebug;

int main(int ac, char **av)
{
    if (ac != 2) {
        return 1;
    }

    node_manager_t nmgr = {0};
    string_set_t smgr = {0};
    kxs_parsectx_t parsectx = {0};
    parsectx.lexctx.fp = fopen(av[1], "r");
    if (!parsectx.lexctx.fp) {
        return 1;
    }

    #if YYDEBUG == 1
    yydebug = 1;
    #endif
    parsectx.node_mgr = &nmgr;
    parsectx.string_mgr = &smgr;
    parsectx.lexctx.ch = ' ';
    int r = yyparse(&parsectx);

    node_free_all(&nmgr);
    return r;
}
