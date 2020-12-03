#include "lexer.h"
#include "kxs.tab.h"

#define lex_is_whitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define lex_is_oct(c) ((c) == '0' || (c) == '1' || (c) == '2' || (c) == '3' || (c) == '4' || (c) == '5' || (c) == '6' || (c) == '7')
#define lex_is_name1(c) (isalpha(c) || (c) == '$' || (c) == '_')
#define lex_is_name2(c) (isalnum(c) || (c) == '$' || (c) == '_')

static int lex_curr(kxs_lexctx_t *lexctx)
{
    return lexctx->ch;
}

static int lex_next(kxs_lexctx_t *lexctx)
{
    if (lexctx->ch == EOF) {
        return EOF;
    }
    int ch = fgetc(lexctx->fp);
    if (ch == EOF) {
        return lexctx->ch = EOF;
    }
    return lexctx->ch = ch;
}

static int get_token_of_keyword(const char *buf)
{
    if (buf[0] == 'd') {
        if (!strcmp(buf, "dbl")) return DBL_TYPE;
    } else if (buf[0] == 'e') {
        if (!strcmp(buf, "else")) return ELSE;
    } else if (buf[0] == 'f') {
        if (!strcmp(buf, "for")) return FOR;
        if (!strcmp(buf, "function")) return FUNCTION;
    } else if (buf[0] == 'i') {
        if (!strcmp(buf, "if")) return IF;
        if (!strcmp(buf, "int")) return INT_TYPE;
    } else if (buf[0] == 'r') {
        if (!strcmp(buf, "return")) return RETURN;
    } else if (buf[0] == 'v') {
        if (!strcmp(buf, "var")) return VAR;
    } else if (buf[0] == 'w') {
        if (!strcmp(buf, "while")) return WHILE;
    }
    return NAME;
}

#define LEX_BUFMAX (256)
int yylex(YYSTYPE *yylval, kxs_parsectx_t *parsectx)
{
    kxs_lexctx_t *lexctx = &(parsectx->lexctx);
    int ch = lex_curr(lexctx);

    // skip whitespaces.
    while (lex_is_whitespace(ch)) {
        ch = lex_next(lexctx);
    }

    if (ch == EOF) {
        return EOF;
    }

    #define LEX_CASE(CH) \
        case CH:\
            lex_next(lexctx);\
            return CH;\
    /**/
    #define LEX_EQCASE(CH, EQVAL) \
        case CH:\
            ch = lex_next(lexctx);\
            if (ch == '=') {\
                lex_next(lexctx);\
                return EQVAL;\
            }\
            return CH;\
    /**/
    switch (ch) {
    LEX_EQCASE('=', EQEQ);
    LEX_EQCASE('!', NEQ);
    LEX_EQCASE('+', NEQ);
    LEX_EQCASE('-', NEQ);
    LEX_EQCASE('*', NEQ);
    LEX_EQCASE('/', NEQ);
    LEX_EQCASE('%', NEQ);
    LEX_EQCASE('<', LEQ);
    LEX_EQCASE('>', GEQ);
    LEX_CASE('(');
    LEX_CASE(')');
    default:
        ;
    }
    #undef LEX_EQCASE

    if (isdigit(ch)) {
        int i = 0;
        char buf[LEX_BUFMAX] = {0};
        if (ch == '0') {
            ch = lex_next(lexctx);
            if (ch == 'x' || ch == 'X') {
                ch = lex_next(lexctx);
                while (i < LEX_BUFMAX && isxdigit(ch)) {
                    buf[i++] = ch;
                    ch = lex_next(lexctx);
                }
                yylval->iv = strtoll(buf, NULL, 16);
                return INT_VALUE;
            } else if (lex_is_oct(ch)) {
                while (i < LEX_BUFMAX && lex_is_oct(ch)) {
                    buf[i++] = ch;
                    ch = lex_next(lexctx);
                }
                yylval->iv = strtoll(buf, NULL, 8);
                return INT_VALUE;
            }
            // just 0.
            yylval->iv = 0;
            return INT_VALUE;
        }
        while (i < LEX_BUFMAX && isdigit(ch)) {
            buf[i++] = ch;
            ch = lex_next(lexctx);
        }
        if (ch != '.') {
            yylval->iv = strtoll(buf, NULL, 10);
            return INT_VALUE;
        }
        ch = lex_next(lexctx);
        while (i < LEX_BUFMAX && isdigit(ch)) {
            buf[i++] = ch;
            ch = lex_next(lexctx);
        }
        if (ch == 'e' || ch == 'E') {
            ch = lex_next(lexctx);
            if (ch == '+' || ch == '-') {
                ch = lex_next(lexctx);
            }
            while (i < LEX_BUFMAX && isdigit(ch)) {
                buf[i++] = ch;
                ch = lex_next(lexctx);
            }
        }
        yylval->dv = strtod(buf, NULL);
        return DBL_VALUE;
    }

    if (lex_is_name1(ch)) {
        int i = 1;
        char buf[LEX_BUFMAX] = {ch};
        ch = lex_next(lexctx);
        while (i < LEX_BUFMAX && lex_is_name2(ch)) {
            buf[i++] = ch;
            ch = lex_next(lexctx);
        }
        return get_token_of_keyword(buf);
    }

    if (isgraph(ch)) {
        lex_next(lexctx);
        return ch;
    }

    return ERROR;
}
