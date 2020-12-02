# はじめに

**「見た目は JavaScript、頭脳(中身)は Ruby、（安定感は AC/DC）」** でお届けしているスクリプト言語 [Kinx](https://github.com/Kray-G/kinx)。Kinx-Static 実装編。今回は BNF を定義します。

* 参考
    * リポジトリ ... https://github.com/Kray-G/kinx
        * Pull Request 等お待ちしております。

# BNF

では、Kinx Static の BNF を定義してみましょう。トップダウンで作っていきます。

尚、ここでは BNF とは言いつつ、yacc の記法で書いていきます。すなわち、そのまま yacc プログラムとなるようにして、パーサージェネレーターでパーサーまで作れることを想定します。つまり、今回から「実装編」となります。

## レキサーとのインタフェース

パーサーはレキサーから値を受け取って処理します。レキサーからは **トークン** という情報を受け取って、そのトークンの並びを判断するのがパーサーの仕事です。Kinx Static では以下のトークンがあります。1 文字で表せないものは代わりとなる記号を用意します。

|     トークン      |           内容            |
| :---------------- | :------------------------ |
| `=` などの 1 文字 | その文字そのものを表す    |
| IF                | `if` の文字列を表す       |
| ELSE              | `else` の文字列を表す     |
| FOR               | `for` の文字列を表す      |
| WHILE             | `while` の文字列を表す    |
| FUNCTION          | `function` の文字列を表す |
| VAR               | `var` の文字列を表す      |
| INT               | `int` の文字列を表す      |
| DBL               | `dbl` の文字列を表す      |
| ADDEQ             | `+=` の文字列を表す       |
| SUBEQ             | `-=` の文字列を表す       |
| MULEQ             | `*=` の文字列を表す       |
| DIVEQ             | `/=` の文字列を表す       |
| MODEQ             | `%=` の文字列を表す       |
| NAME              | 変数名を表す              |
| INT_VALUE         | 整数値を表す              |
| DBL_VALUE         | 実数値を表す              |

トークンは通常整数値です。上記のトークンは以下の定義の中で追々出てきますが、上記の意味だと思ってください。パーサー内では、トークンは **終端記号** として扱われます。終端記号についても後述します。

## program

`program` は文の連続です。文の連続は `statement_list` としてまとめておくことにすれば、こんな感じですね。

```nbnf
program
    : statement_list
    ;
```

簡単ですね。

尚、ここで出てくる `program` とか `statement_list` とかを **非終端記号** と呼びます。何が非終端なのか、というのは追々説明します。

## statement_list

`statement_list` は `statement` の連続を表します。BNF では以下のように 2 行で書きます。左再帰の形で、これは以下のことを意味します。

* `statement` は `statement_list` ですよ。
* `statement_list` の後に `statement` が来たものも `statement_list` ですよ。

```nbnf
statement_list
    : statement
    | statement_list statement
    ;
```

この定義によって、例えばこんな感じでインプットがあったとき、

```
[statement] [statement] [statement] [statement] [statement]
```

以下のように順々に `statement_list` に置き換わっていきます。

```html
[statement] [statement] [statement] [statement] [statement]
    ↓
<statement> [statement] [statement] [statement] [statement]
    │ <statement> → [statement_list]
    ↓
[statement_list] [statement] [statement] [statement] [statement]
    ↓
<statement_list statement> [statement] [statement] [statement]
    │ <statement_list statement> → [statement_list]
    ↓
[statement_list] [statement] [statement] [statement]
    ↓
<statement_list statement> [statement] [statement]
    │ <statement_list statement> → [statement_list]
    ↓
[statement_list] [statement] [statement]
    ↓
<statement_list statement> [statement]
    │ <statement_list statement> → [statement_list]
    ↓
[statement_list] [statement]
    ↓
<statement_list statement>
    │ <statement_list statement> → [statement_list]
    ↓
[statement_list]
```

こんな感じで `statement` が来るたびに順に `statement_list` に吸収されていき、最終的に `statement_list` に収斂していくイメージが湧けばOKです。

ちなみにこの動作を reduce と言います。日本語では還元と言います。要素が減って収斂していくイメージですね。

## statement

`statement` は以下の種類があります。

* 宣言文
* 式文（代入文を含む）
* if 文
* for 文
* while 文
* 関数定義
* `{` と `}` で括られた文(`statement`)の集合

これを BNF に表すと以下のようになります。

```nbnf
statement
    : declaration_statement
    | expression_statement
    | if_statement
    | for_statement
    | while_statement
    | function_definition
    | block
    ;

block
    : '{' statement_list '}'
    ;
```

block は `statement_list` を利用して再帰的に定義しています。意味は分かりますね。では、block 以外、それぞれを定義していきましょう。

### 宣言文

宣言文は、以下のような文になります。

```javascript
var a;
var a, b;
var a = 1, b;
var a, b = 2.0;
var a:int = 1, b = 2.0;
```

特徴を書いてみると、以下のような感じですね。

* `var` で始まる。
* `変数名:型 = 初期値` の形で、`:型` と `= 初期値` は省略可能。
* `,` で区切って複数の変数を宣言できる。

では、これを BNF で定義してみましょう。省略可能なものは、別で定義すると定義しやすいです。一応命名ルールとして、`_Opt` が最後につく非終端記号は省略可能なものとして扱うことにします。尚、宣言文は `';'` で終ります。

```nbnf
declaration_statement
    : var_declaration ';'
    ;

var_declaration
    : VAR declaration_list
    ;

declaration_list
    : declaration_expression
    | declaration_list ',' declaration_expression
    ;

declaration_expression
    : NAME type_info_Opt initializer_Opt
    ;
```

3 つに分かれましたが、大体わかりますかね。

ここで `','` や `VAR`、`NAME` は **終端記号** になります。ここではシングルクォートに括られた1文字、または大文字で表現されたものを終端記号として扱うことにします（必ずしもそういうルールではありませんが、一般的にはそうすると分かりやすいです）。

これで非終端記号と終端記号の 2 つが出そろいましたので、説明しておきましょう。

終端記号というのはレキサー（スキャナーとも言う、字句解析器）が返す値、つまりトークンを指します。通常は Integer で表現され、レキサーとパーサー間で情報をやり取りするために定義された値です。**終端** の意味は、これ以上分解できない要素なのでそう呼ばれます。尚、シングルクォートに括られた1文字が終端記号となることができるのは、C言語ではそれが Integer だからで、yacc/Bison などのパーサージェネレーターでは、`NAME` のような終端記号は通常 256 より大きい数値が割り当てられて、char 1 文字の値とは重ならないようになっています。

一方、非終端記号というのはパーサーの中での一つの状態で、パーサーに閉じた値となります。これは、別の非終端記号や終端記号によって内容を定義する必要があるため、**非終端** と呼ばれます。

では、`declaration_statement` の構成要素のうち、非終端記号の部分を定義していきましょう。

```nbnf
type_info_Opt
    : /* empty */
    | ':' type_name
    ;

type_name
    : INT
    | DBL
    ;

initializer_Opt
    : /* empty */
    | '=' expression
    ;
```

`expression` は式文で出てくるので一旦保留して、だいたい理解可能でしょうか。

`/* empty */` は何も書かなくても良いのですが、コメント形式で書いておくことによって空集合も `type_info_Opt` や `initializer_Opt` に収斂させていくことができるようになります。これはつまり、**省略を許容する** ことを意味します。無くても文法上間違いではない、ということです。

現時点で型として認識するものは `INT` と `DBL` のみです。これはレキサーが `int`、`dbl` を認識したときに返ってくる値です。

### 式文（代入文を含む）

式文を定義しましょう。式文は優先順位に従って定義すると、曖昧さがなくなっていい感じになります。言っている意味が分からないかもしれませんが、まずは具体的な例を見てみましょう。今回サポートするのは加減乗除と剰余のみです。代入を含めると `=`、`+=`、`-=`、`*=`、`/=`、`%=` が加わりますが、例えば `a += b` は `a = a + b` と同等の扱いです。 

演算子の優先度は以下の通りです。

| 優先度 |         内容         |              演算子               | 評価  |
| :----: | :------------------- | :-------------------------------- | :---- |
|   高   | 要素                 | 変数名、整数値(int)、実数値(dbl)  | -     |
|   ↑    | 乗算、除算、剰余演算 | `*`、`/`、`%`                     | 左→右 |
|   ↓    | 加算、減算           | `+`、`-`                          | 左→右 |
|   低   | 代入                 | `=`、`+=`、`-=`、`*=`、`/=`、`%=` | 右→左 |

この優先順位に従って BNF を書くと以下の通りになります。出発地点は `expression_statement` です。式文も `';'` で終ります。表現にもだいぶ慣れてきたと思いますので、一気に行きましょう。なお、代入文も左再帰の形で書いていますが、AST（Abstract Syntax Tree = 抽象構文木）を作成する際に右再帰のツリー構造として組めば問題ありません[^rrec]。

[^rrec]: 右再帰を多用するとパーサーのスタックがあふれる可能性があるので、こうしています。別に右再帰で書いてもいいのですが。

```nbnf
expression_statement
    : expression ';'
    ;

expression
    : assign_expression
    ;

assign_expression
    : add_sub_expression
    | assign_expression '=' add_sub_expression
    | assign_expression ADDEQ add_sub_expression
    | assign_expression SUBEQ add_sub_expression
    | assign_expression MULEQ add_sub_expression
    | assign_expression DIVEQ add_sub_expression
    | assign_expression MODEQ add_sub_expression
    ;

add_sub_expression
    : mul_div_mod_expression
    | add_sub_expression '+' mul_div_mod_expression
    | add_sub_expression '-' mul_div_mod_expression
    ;

mul_div_mod_expression
    : factor
    | mul_div_mod_expression '*' factor
    | mul_div_mod_expression '/' factor
    | mul_div_mod_expression '%' factor
    ;

factor
    : NAME
    | INT_VALUE
    | DBL_VALUE
    | '(' expression ')'
    ;
```

`factor` と `'(' expression ')'` の関係は、`statement` と `block` の関係と同じです。このように再帰的に定義されるものも結構あります。

また、優先度の高いものが下に来ています（下に書かなければならないわけではない）。これは動きを考えてみればわかります。パーサーは終端記号から順に非終端記号に reduce されていきます。つまり、この例では下にある要素としてまずまとめられた後、上に記載された非終端記号に reduce されていくことになります。実際の式で見てみましょう。`1 + 2 * x + y` を上記に従って構文解析してみます。ちょっと長いですが、理解はできるかな、と思います。

```html
1 + 2 * x + y
    ↓
<INT_VALUE> + 2 * x + y
    | <INT_VALUE> → [factor]
    ↓
[factor] + 2 * x + y
    | [factor] → [mul_div_mod_expression]
    ↓
[mul_div_mod_expression] + 2 * x + y
    | [mul_div_mod_expression] → [add_sub_expression]
    |    ※ 次の文字 `+` が `*` でも `/` でも `%` でもないため
    ↓
[add_sub_expression] + 2 * x + y
    | `+` を取り込む ...この動作を shift という
    ↓
<add_sub_expression +> 2 * x + y
    ↓
<add_sub_expression +> <INT_VALUE> * x + y
    | <INT_VALUE> → [factor]
    ↓
<add_sub_expression +> [factor] * x + y
    | [factor] → [mul_div_mod_expression]
    ↓
<add_sub_expression +> [mul_div_mod_expression] * x + y
    | `*` を取り込む
    ↓
<add_sub_expression +> <mul_div_mod_expression *> x + y
    ↓
<add_sub_expression +> <mul_div_mod_expression *> <NAME> + y
    | <NAME> → [factor]
    ↓
<add_sub_expression +> <mul_div_mod_expression *> [factor] + y
    | [factor] を取り込む
    ↓
<add_sub_expression +> <mul_div_mod_expression * factor> + y
    | <mul_div_mod_expression * factor> → [mul_div_mod_expression]
    ↓
<add_sub_expression +> [mul_div_mod_expression] + y
    | [mul_div_mod_expression] を取り込む
    ↓
<add_sub_expression + mul_div_mod_expression> + y
    | <add_sub_expression + mul_div_mod_expression> → [add_sub_expression]
    ↓
[add_sub_expression] + y
    | `+` を取り込む
    ↓
<add_sub_expression +> y
    ↓
<add_sub_expression +> <NAME>
    | <NAME> → [factor]
    ↓
<add_sub_expression +> [factor]
    | [factor] → [mul_div_mod_expression]
    ↓
<add_sub_expression +> [mul_div_mod_expression]
    | [mul_div_mod_expression] を取り込む
    ↓
<add_sub_expression + mul_div_mod_expression>
    | <add_sub_expression + mul_div_mod_expression> → [add_sub_expression]
    ↓
[add_sub_expression]
    | [add_sub_expression] → [assign_expression]
    ↓
[assign_expression]
    | [assign_expression] → [expression]
    ↓
[expression]
```

ちゃんと乗算が先に行われたのが分かります。

なお、途中で「次の文字 `+` が `*` でも `/` でも `%` でもないため」という注意書きがありましたが、このように yacc では 1 文字（正確には 1 トークン）だけ先読みすることで構文解析を行います。

### if 文

if 文の定義は以下の通りです。実はこの辺の制御構造は `expression` や `statement` が定義されていれば特段難しくなかったりします。

```nbnf
if_statement
    : IF '(' expression ')' statement else_clause_Opt
    ;

else_clause_Opt
    : /* empty */
    | ELSE statement
    ;
```

* `statement` は `block` も含む。
* else 節は省略可能。
* 上記の定義だけでいわゆる **ぶら下がり構文** も実現可能。`if ～ else if ～` の形。

特段難しくないのですが、if に限っては else の扱いで一つ問題が発生します。

ここでは else 節を省略可能にしていますが、これが問題になります。一般に、shift/reduce コンフリクトという警告が発生します。shift/reduce コンフリクトとは、shift もできるし reduce もできる、という状況が発生した、ということです。

では何が問題になるか見てみましょう。書いてみましょう。

```javascript
if (expression1)
    if (expression2)
        statement1
    else
        statement2
```

これはインデントで明確なようにも見えますが、実は次の意味にもとれます。

```javascript
if (expression1)
    if (expression2)
        statement1
else
    statement2
```

つまり、else が認識された時点で内側の `else_clause_Opt` は `/* empty */` と認識することも可能なのです。なぜならば、最初の `if (expression1)` があることで一旦 `if (expression2) statement1` だけで `if_statement` → `statement` に reduce できてしまうからですね。

ただ、yacc の場合、通常このようなケースでの shift/reduce コンフリクトは無視して構いません。実はデフォルトでは shift に倒されます。つまり、最初のインデントの例のように内側の if に対する else として動作します。これは期待通りの動きです。なので、shif/reduce コンフリクトの際は問題かどうかを検討して、問題なければそのままでも良いケースも多いと思っていればよいでしょう。なぜならば、shift 動作というのはもっとも近いものとペアになる動作なので、概ね直観と合う動作になるからです。

一方で、reduce/reduce コンフリクトというのもあります。これは 2 つある定義のどちらに reduce してよいかわからない（どちらにも reduce できる）という状態で、これは文法に無理があることを示しています。なので、文法定義を見直す必要があります。

### for 文

for 文もそんなに難しくありません。いくつかパターンがありますが、コンフリクトはないでしょう。

```nbnf
for_statement
    : FOR
      '(' for_expression1_Opt ';' for_expression2_Opt ';' for_expression2_Opt ')'
      statement
    ;

for_expression1_Opt
    : /* empty */
    | expression
    | var_declaration
    ;

for_expression2_Opt
    : /* empty */
    | expression
    ;
```

for 文の最初の式だけ宣言式（`var ...`）が可能です。また、全ての式は省略可能です。全て省略した場合は無限ループを意味します。

### while 文

while 文は全く簡単です。

```nbnf
while_statement
    : WHILE '(' expression ')' statement
    ;
```

これだけで while の構文がサポートできます。

### 関数定義

さて、関数定義です。関数定義は以下のような形で、型情報は省略可能です。

```javascript
function:int func(a:int, b:int) {
    ...
}
```

関数の中はいわゆる `statement_list` ですね。これを素直に BNF にしてみましょう。

```nbnf
function_definition
    : FUNCTION type_info_Opt NAME '(' argument_list ')' block
    ;

argument_list
    : argument
    | argument_list ',' argument
    ;

argument
    : NAME type_info_Opt
    ;
```

今まで定義したものを再利用して、あっさり定義できました。

## BNF 全体と yacc によるパーサー生成

ここまでで、`kinxstatic.y` ファイルとして保存して yacc（または bison）にかけてみましょう。私は色んな意味で `kmyacc` を使います。理由は [こちら](https://qiita.com/Kray-G/items/c77f2d5401c31290bfcd)。

<details><summary>`kinxstatic.y`</summary><div>

<pre><code>%token IF ELSE FOR WHILE FUNCTION
%token VAR NAME INT DBL ADDEQ SUBEQ MULEQ DIVEQ MODEQ INT_VALUE DBL_VALUE

%%

program
    : statement_list
    ;

statement_list
    : statement
    | statement_list statement
    ;

statement
    : declaration_statement
    | expression_statement
    | if_statement
    | for_statement
    | while_statement
    | function_definition
    | block
    ;

block
    : '{' statement_list '}'
    ;

declaration_statement
    : var_declaration ';'
    ;

var_declaration
    : VAR declaration_list
    ;

declaration_list
    : declaration_expression
    | declaration_list ',' declaration_expression
    ;

declaration_expression
    : NAME type_info_Opt initializer_Opt
    ;

type_info_Opt
    : /* empty */
    | ':' type_name
    ;

type_name
    : INT
    | DBL
    ;

initializer_Opt
    : /* empty */
    | '=' expression
    ;

expression_statement
    : expression ';'
    ;

expression
    : assign_expression
    ;

assign_expression
    : add_sub_expression
    | assign_expression '=' add_sub_expression
    | assign_expression ADDEQ add_sub_expression
    | assign_expression SUBEQ add_sub_expression
    | assign_expression MULEQ add_sub_expression
    | assign_expression DIVEQ add_sub_expression
    | assign_expression MODEQ add_sub_expression
    ;

add_sub_expression
    : mul_div_mod_expression
    | add_sub_expression '+' mul_div_mod_expression
    | add_sub_expression '-' mul_div_mod_expression
    ;

mul_div_mod_expression
    : factor
    | mul_div_mod_expression '*' factor
    | mul_div_mod_expression '/' factor
    | mul_div_mod_expression '%' factor
    ;

factor
    : NAME
    | INT_VALUE
    | DBL_VALUE
    | '(' expression ')'
    ;

if_statement
    : IF '(' expression ')' statement else_clause_Opt
    ;

else_clause_Opt
    : /* empty */
    | ELSE statement
    ;

for_statement
    : FOR
      '(' for_expression1_Opt ';' for_expression2_Opt ';' for_expression2_Opt ')'
      statement
    ;

for_expression1_Opt
    : /* empty */
    | expression
    | var_declaration
    ;

for_expression2_Opt
    : /* empty */
    | expression
    ;

while_statement
    : WHILE '(' expression ')' statement
    ;

function_definition
    : FUNCTION type_info_Opt NAME '(' argument_list ')' block
    ;

argument_list
    : argument
    | argument_list ',' argument
    ;

argument
    : NAME type_info_Opt
    ;

%%
</code></pre>

</div></details>

では、yacc を使ってパーサーを作ってみましょう。ちゃんとできるかな？

```
$ kmyacc -v -d -b kxs kinxstatic.y
kinxstatic.y: there are 1 shift/reduce conflicts
```

先ほどの shift/reduce コンフリクトですね。それが 1 つだけなので、成功です。具体的に .output ファイルを見てみると以下の指摘が見つかります。

```
state 45
45: shift/reduce conflict (shift 6, reduce 44) on ELSE
	(43) if_statement : IF '(' expression ')' statement . else_clause_Opt
	(44) else_clause_Opt : .

	ELSE		shift 6
	else_clause_Opt	goto 96 and reduce (43)
	.		reduce (44)
```

`ELSE` で shift するか、`else_clause_Opt` に reduce するかでちゃんと悩んでますねー。

# おわりに

現時点でのミニマム仕様で BNF が定義できました。次は AST（抽象構文木）を作ります。

では、また。
