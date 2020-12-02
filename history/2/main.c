#include "lexer.h"

extern int yyparse(kxs_parsectx_t *);
extern int yydebug;

int main(int ac, char **av)
{
    if (ac != 2) {
        return 1;
    }

    kxs_parsectx_t parsectx;
    parsectx.lexctx = (kxs_lexctx_t*)calloc(1, sizeof(kxs_lexctx_t));
    parsectx.lexctx->fp = fopen(av[1], "r");
    if (!parsectx.lexctx->fp) {
        return 1;
    }

    yydebug = 1;
    parsectx.lexctx->ch = ' ';
    int r = yyparse(&parsectx);

    free(parsectx.lexctx);
    return 0;
}
