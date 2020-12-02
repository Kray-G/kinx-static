# はじめに

**「見た目は JavaScript、頭脳(中身)は Ruby、（安定感は AC/DC）」** でお届けしているスクリプト言語 [Kinx](https://github.com/Kray-G/kinx)。Kinx-Static 実装編。今回はレキサーを作ります。

* 参考
    * リポジトリ ... https://github.com/Kray-G/kinx
        * Pull Request 等お待ちしております。

# lexer（レキサー、字句解析器）

前回、BNF を定義してパーサージェネレーターでパーサーが作れそうなところまで行きました。今回は、パーサーが動かせるようにレキサーを作ります。

## トークン

パーサーを動かすにはレキサー（字句解析器）が必要ですね。でないとパーサーへのインプットがありません。レキサーが返すトークンは、以下の通りです。

|     トークン      |           内容            |
| :---------------- | :------------------------ |
| `=` などの 1 文字 | その文字そのものを表す    |
| IF                | `if` の文字列を表す       |
| ELSE              | `else` の文字列を表す     |
| FOR               | `for` の文字列を表す      |
| WHILE             | `while` の文字列を表す    |
| FUNCTION          | `function` の文字列を表す |
| VAR               | `var` の文字列を表す      |
| INT_TYPE          | `int` の文字列を表す      |
| DBL_TYPE          | `dbl` の文字列を表す      |
| ADDEQ             | `+=` の文字列を表す       |
| SUBEQ             | `-=` の文字列を表す       |
| MULEQ             | `*=` の文字列を表す       |
| DIVEQ             | `/=` の文字列を表す       |
| MODEQ             | `%=` の文字列を表す       |
| EQEQ              | `==` の文字列を表す       |
| NEQ               | `!=` の文字列を表す       |
| LEQ               | `<=` の文字列を表す       |
| GEQ               | `>=` の文字列を表す       |
| NAME              | 変数名を表す              |
| INT_VALUE         | 整数値を表す              |
| DBL_VALUE         | 実数値を表す              |

これらの値は、実は yacc にヘッダファイルとして出力してくれる機能があります。前回の結果で `kxs.tab.h` というファイルがあるかと思いますので、中を見てみましょう。

```c
#ifndef YYSTYPE
#define YYSTYPE int
#endif
extern YYSTYPE yylval;

#define YYERRTOK 256
#define ERROR 257
#define IF 258
#define ELSE 259
#define FOR 260
#define WHILE 261
#define FUNCTION 262
#define RETURN 263
#define ADDEQ 264
#define SUBEQ 265
#define MULEQ 266
#define DIVEQ 267
#define MODEQ 268
#define EQEQ 269
#define NEQ 270
#define LEQ 271
#define GEQ 272
#define VAR 273
#define NAME 274
#define INT_TYPE 275
#define DBL_TYPE 276
#define INT_VALUE 277
#define DBL_VALUE 278
```

今後の実装で変化していきますが、一先ずレキサーはこのヘッダを include することが可能です。

## 実装

レキサーの実装をしていきましょう。

### トークナイズ戦略

字句解析は手書きでも難しくないので、変にライブラリやジェネレーターは使わずに、以下のやり方で行います。

* 1 文字先読み
* 以下の順に分類していきトークンを決定
    * 記号
        * 1 文字の記号 ... 文字そのものを返す
        * 2 文字の記号 ... ADDEQ などのトークンを返す
    * 数値
        * 整数 ... INT_VALUE を返す（10進数、16進数、8進数をサポートする）
        * 実数 ... DBL_VALUE を返す
    * 名前
        * 予約語 ... IF などのトークンを返す
        * その他 ... NAME を返す

### yylval

レキサーからパーサーに渡せる値はトークンだけではありません。例えば、INT_VALUE のときの実際の数値情報なども渡せなければなりません。そのために yylval という共用体を使います。

共用体の定義は .y ファイルの最初の `%%` より前に以下のように記載します。今回は整数値と実数値のみなので、以下で良いでしょう。

```bison
%union {
    int64_t iv;
    double dv;
}
```
`kxs.tab.h` は以下のように変わります。共用体の型が `YYSTYPE` として定義されました。

```c
typedef union
{
    int64_t iv;
    double dv;
}
YYSTYPE;
extern YYSTYPE yylval;

#define YYERRTOK 256
#define ERROR 257
#define IF 258
#define ELSE 259
#define FOR 260
#define WHILE 261
#define FUNCTION 262
#define RETURN 263
#define ADDEQ 264
#define SUBEQ 265
#define MULEQ 266
#define DIVEQ 267
#define MODEQ 268
#define EQEQ 269
#define NEQ 270
#define LEQ 271
#define GEQ 272
#define VAR 273
#define NAME 274
#define INT_TYPE 275
#define DBL_TYPE 276
#define INT_VALUE 277
#define DBL_VALUE 278
```

### yylex

yacc ではレキサーからのインタフェースは `yylex` という関数名でアクセスされるので、yylex 関数を定義します。

また、YYLEX_PARAM マクロを定義しておくと yylex 関数に引数を渡せます。ここでは kxs_parsectx_t 型へのポインタを受け取るようにしておきます。ここには次の読み込み用のパラメータなどを保持できるようにしておき、`lex_curr(ctx)` で現在の文字を、`lex_next(ctx)` で次の文字にフェッチできるものとします。

尚、引数を受け取るようにする場合、.y ファイルに `%pure_parser` という指定をしておく必要があります。

```c
int yylex(kxs_parsectx_t *parsectx)
{
    ...
}
```

kxs_lexctx_t 型、kxs_parsectx_t 型の定義は .y ファイルの最初の `%%` より前に以下のように記載します。また、`%pure_parser` の指定もしておきます。

```bison
%pure_parser

%{
#include <stdint.h>
#include <stdio.h>

typedef struct kxs_lexctx_t_ {
    int ch;
    FILE *fp;
} kxs_lexctx_t;

typedef struct kxs_parsectx_t_ {
    kxs_lexctx_t lexctx;
    char *file;
    int line
} kxs_parsectx_t;

#define YYPARSE_PARAM parsectx
#define YYLEX_PARAM parsectx
%}
```

パーサーのコンテキストを保持するパラメータの中にレキサーのコンテキストを含めておきます。パーサーのコンテキストには現在処理しているファイル名と行番号を保持できるようにしておきます。

### yyerror

パーサーはエラー発生時に `yyerror` という関数を呼びます。一先ず以下のように定義しておきます。定義場所は、.y ファイルの 2 つ目の `%%` より下の部分に記載します。この場所に記載することで、生成された C ソースの一番下にそのまま追加されます。

```c
int yyerror(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    return 0;
}
```

### ソースコード

さて、ソースコードとして整えたファイルは以下にあります。

* [kinxstatic.y](https://github.com/Kray-G/kinx-static/blob/main/history/2/kinxstatic.y)
* [lexer.c](https://github.com/Kray-G/kinx-static/blob/main/history/2/lexer.c)
* [lexer.h](https://github.com/Kray-G/kinx-static/blob/main/history/2/lexer.h)

### お試し

ここまで用意できると、文法が正しく受理されるかどうかを確認できます。トレースを出力するように設定して動作させてみましょう。
トレース機能を ON にするには、`YYDEBUG` マクロを 1 にしてコンパイルし、yydebug を 1 にして実行することで可能です。

簡易的な main 関数を以下のように定義して使ってみましょう。

```c
#include "lexer.h"

extern int yyparse(kxs_parsectx_t *);
extern int yydebug;

int main(int ac, char **av)
{
    if (ac != 2) {
        return 1;
    }

    kxs_parsectx_t parsectx;
    parsectx.lexctx.fp = fopen(av[1], "r");
    if (!parsectx.lexctx.fp) {
        return 1;
    }

    #if YYDEBUG == 1
    yydebug = 1;
    #endif
    parsectx.lexctx.ch = ' ';
    int r = yyparse(&parsectx);

    return 0;
}
```

実行ファイル名は悩んだ挙句 `kiss` にしてあります。んー。**Ki**nx **S**tatic **S**ubset の略です。
`sample.kxs` というファイルを作って実行してみます。`sample.kxs` は以下のようなフィボナッチ関数です。

```javascript
function fib(n) {
    if (n < 3) return n;
    return fib(n-2) + fib(n-1);
}

return fib(34);
```

これを実行してみましょう。

```
$ ./kiss sample.kxs
%% State 0, Lookahead --none--
%% Reading FUNCTION
%% Shift FUNCTION
%% State 58, Lookahead --none--
%% Reading NAME
%% Reduce by (18) type_info_Opt : /* empty */
%% State 73, Lookahead NAME
%% Shift NAME
%% State 80, Lookahead --none--
%% Reading '('
%% Shift '('
%% State 57, Lookahead --none--
...
%% Reduce by (25) expression : assign_expression
%% State 74, Lookahead ';'
%% Shift ';'
%% Reduce by (65) return_statement : RETURN expression ';'
%% Reduce by (9) statement : return_statement
%% Reduce by (3) statement_list : statement_list statement
%% State 3, Lookahead --none--
%% Reading $EOF
%% Reduce by (1) program : statement_list
%% State 69, Lookahead $EOF
%% Accepted.
```

`Accepted.` と出れば、正しくパースできたことを意味します。

### エラー処理

簡単なエラー処理だけ入れておきましょう。通常、エラーが発生してもある程度復帰して、なるべく多くのエラーを一気に出しておきたいところです。このような場合に良く使われるのが **パニックモード** と呼ばれる方法です。パニックモードでは、例えば `';'` などの特定のトークンが現れるまで読み捨てることで復旧を試みます。yacc では error という特別なトークンを用いて実現が可能です。

`statement` の定義を以下のように書き換えます。

```nbnf
statement
    : declaration_statement
    | expression_statement
    | if_statement
    | for_statement
    | while_statement
    | return_statement
    | function_definition
    | block
    | error ';'
    ;
```

これによって、エラーが発生した場合、その時点からトークンが読み飛ばされ、';' が出現した時点で `statement` に reduce されます。色々なケースを考えるとこれだけでは不十分なのですが、一番簡単な方法としてまずはこれで良いでしょう。

# おわりに

レキサーを実装したことでパーサーが動き始めました。このように少しずつでも動作しているものを拡張していくほうが楽しいですね。次はパーサーの出力として AST を作るところまで行きましょう。

ではまた。
