# はじめに

**「見た目は JavaScript、頭脳(中身)は Ruby、（安定感は AC/DC）」** でお届けしているスクリプト言語 [Kinx](https://github.com/Kray-G/kinx)。Kinx-Static 実装編。今回は AST を作ります。

* 参考
    * 最初の動機 ... [スクリプト言語 KINX（ご紹介）](https://qiita.com/Kray-G/items/ca08b6fb40d15dd0ec76)
        * 個別記事へのリンクは全てここに集約してあります。
    * リポジトリ ... https://github.com/Kray-G/kinx
        * Pull Request 等お待ちしております。
    * リポジトリ ... https://github.com/Kray-G/kinx-static
        * Kinx-Static のリポジトリができました。

ところで現時点で Kinx と Kinx-Static に接点がありません。Kinx-Static の成果を Kinx にマージできないかと思案しています。

# AST - Abstract Syntax Tree（抽象構文木）

前回、BNF を定義してパーサージェネレーターでパーサーが作れそうなところまで行きました。今回は、そこから AST を作ります。

## AST とは

Abstract Syntax Tree、日本語では抽象構文木と言います。構文解析をした後、その結果をツリーとして構築したデータ構造のことです。詳しくは以下の Wikipedia へ。

* https://ja.wikipedia.org/wiki/抽象構文木

## lexer（レキサー、字句解析器）

### トークン

AST を作る前に、レキサー（字句解析器）が必要ですね。でないとパーサーへのインプットがありません。レキサーが返すトークンは、以下の通りです。

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

### 実装

レキサーの実装をしていきましょう。

#### トークナイズ戦略

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

#### yylval

レキサーからパーサーに渡せる値はトークンだけではありません。例えば、INT_VALUE のときの実際の数値情報なども渡せなければなりません。そのために yylval という共用体を使います。

共用体の定義は .y ファイルの最初の `%%` より前に以下のように記載します。今回は整数値と実数値のみなので、以下で良いでしょう。

```bison
%union {
    int64_t iv;
    double dv;
}
```

yacc ではレキサーからのインタフェースは `yylex` という関数名でアクセスされるので、yylex 関数を定義します。

また、YYLEX_PARAM マクロを定義しておくと yylex 関数に引数を渡せます。ここでは kxs_parsectx_t 型へのポインタを受け取るようにしておきます。ここには次の読み込み用のパラメータなどを保持できるようにしておき、`lex_curr(ctx)` で現在の文字を、`lex_next(ctx)` で次の文字にフェッチできるものとします。

```c
int yylex(kxs_parsectx_t *lexctx)
{
    ...
}
```

kxs_lexctx_t 型、kxs_parsectx_t 型の定義は .y ファイルの最初の `%%` より前に以下のように記載します。

```bison
%{
#include <stdint.h>
#include <stdio.h>

typedef struct kxs_lexctx_t_ {
    int ch;
    FILE *fp;
} kxs_lexctx_t;

typedef struct kxs_parsectx_t_ {
    kxs_lexctx_t *lexctx;
} kxs_parsectx_t;

#define YYPARSE_PARAM parsectx
#define YYLEX_PARAM parsectx
%}
```

パーサーのコンテキストを保持するパラメータの中にレキサーのコンテキストを含めておきます。

#### ソースコード

さて、ソースコードとして整えたファイルは以下にあります。

* [lexer.c](https://github.com/Kray-G/kinx-static/blob/main/history/2/lexer.c)
* [lexer.h](https://github.com/Kray-G/kinx-static/blob/main/history/2/lexer.h)

## AST の作成

### Node


# おわりに


