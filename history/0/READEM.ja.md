# はじめに

**「見た目は JavaScript、頭脳(中身)は Ruby、（安定感は AC/DC）」** でお届けしているスクリプト言語 [Kinx](https://github.com/Kray-G/kinx)。Kinx-Static 設計編。まずは全体的なデザインから。

# 設計

* 以下が初期実装としてサポートされる予定。
    * 型（int、dbl のみ, 省略可）、宣言
    * 式、演算（加減乗除＋剰余）
    * 制御構文（if, while, for）
    * 関数

## 文法

### 宣言

宣言は以下の通り。型として指定できるのは `int` と `dbl` のみ。何も指定していない場合は `int` と解釈される。もしくは右辺値の結果が採用される。また、初期値が指定されていない場合は各型に対するデフォルト値（`int` であれば 0）を設定するものとする。

```javascript
var a;                         // a = 0
var a:int;                     // a = 0
var a = 10;                    // a = 10
var a:int = 11;                // a = 11
var a = 10, b:int = 20;        // a = 10, b = 20
var a:int = 11, b = 21;        // a = 11, b = 21
var a:int = 11, b:dbl = 21;    // a = 11, b = 21.0
var b = 2.0;                   // b = 2.0
```

なお、上記は初期値を数値で指定しているが、任意の式で指定することが可能。

```javascript
var x:int = a + b * 2;
```

### 式

式は、加減乗除演算および剰余演算のみサポート。除算に関しては、結果が自動的に dbl になる。

```javascript
a = b + c;  // b:int,c:int -> a:int
a = b - c;  // b:int,c:int -> a:int
a = b * c;  // b:int,c:int -> a:int
a = b / c;  // b:int,c:int -> a:dbl, this operation will automatically set `a` to double.
a = b % c;  // b:int,c:int -> a:int
```

### 制御構文

#### if

`if` はぶら下がり構文とする。

```javascript
if (expression) {
   ...
} else if (expression) {
   ...
} else {
   ...
}
```

#### while

`while` は通常の C と同じ構文とする。

```javascript
while (expression) {
   ...
}
```

#### for

`for` も通常の C と同じ構文とする。各要素は省略可能であり、省略された場合は何もしない、または省略した条件式は true と解釈する。

```javascript
for (expression; expression; expression) {
   ...
}
```

最初の式は宣言（`var ...`）とすることも可能。

```javascript
for (decralation; expression; expression) {
   ...
}
```

### 関数

関数は以下の通り。

```javascript
function:int functionName(a:int, b:int) {
    ...
    return a + b;
}
```

関数からは `return` で復帰します。

# おわりに

さて、ざっくり設計したので、これを実装していきます。途中、「これではうまくいかんな」と思った場合は仕様変更する感じで、まずは進めていきます。

今後の大きく何かしなきゃいけなくなる想定としては、クロージャをサポートするようになるとフレーム管理が必要になりそうなので、その対応ですかね。変数をフレームに持たせなくてはなりません。そうすると上記のような形でローカル変数に変換できませんね。全ての関数でフレーム処理を入れると激遅になりそうなので、必要な時だけフレームを使うなど、何か対策を考えましょう。

