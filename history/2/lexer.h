#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

typedef struct kxs_lexctx_t_ {
    int ch;
    FILE *fp;
} kxs_lexctx_t;

typedef struct kxs_parsectx_t_ {
    kxs_lexctx_t *lexctx;
} kxs_parsectx_t;
