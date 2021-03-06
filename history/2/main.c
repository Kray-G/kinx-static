#include "lexer.h"

extern int yyparse(kxs_parsectx_t *);
extern int yydebug;

int main(int ac, char **av)
{
    if (ac != 2) {
        return 1;
    }

    kxs_parsectx_t parsectx = {0};
    parsectx.lexctx.fp = fopen(av[1], "r");
    if (!parsectx.lexctx.fp) {
        return 1;
    }

    #if YYDEBUG == 1
    yydebug = 1;
    #endif
    parsectx.lexctx.ch = ' ';
    int r = yyparse(&parsectx);

    return r;
}
