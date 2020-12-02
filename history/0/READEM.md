# Introduction

The scripting language Kinx with a catch phrase of **Looks like JavaScript, feels like Ruby, and Stable like AC/DC(?)**.
This time it is a design of Kinx-Static.

# Design

* Here are the initial features to be supported.
    * Type of `int`, `dbl`, and its declaration.
    * expression of add/sub/mul/div/mod.
    * Flow control by `if`, `while`, and `for`.
    * Function definition.

## Syntax

### Declaration

Here are declaration examples.
You can use `int` and `dbl` only.
If omitting a type, it will be `int` or the type of right hand side.
And you can use an initializer to initialize a value, but it will be a default value for its type if an initializer is missing.
For example, 0 for `int`.

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

By the way, the integer value is specified in the above example, but you can specify any expression like the following.

```javascript
var x:int = a + b * 2;
```

### Expression

As expression it supports addition, subtraction, multiply, division, and modulo.
If it is division, the type of the result will be `dbl` sutomatically.

```javascript
a = b + c;  // b:int,c:int -> a:int
a = b - c;  // b:int,c:int -> a:int
a = b * c;  // b:int,c:int -> a:int
a = b / c;  // b:int,c:int -> a:dbl, this operation will automatically set `a` to double.
a = b % c;  // b:int,c:int -> a:int
```

### Flow Control

#### if

`if` can use a dangling-else style.

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

`while` is the same style with normal C syntax.

```javascript
while (expression) {
   ...
}
```

#### for

`for` is also the same style with normal C syntax.
Each expression can be omitted.
If a condition expression is omitted, it is `true` and it means a infinite loop.

```javascript
for (expression; expression; expression) {
   ...
}
```

At the place of the first expression, `var ...` style as a declaration can be also used.

```javascript
for (decralation; expression; expression) {
   ...
}
```

### Function Definition

The style of a function definition is shown below.

```javascript
function:int functionName(a:int, b:int) {
    ...
}
```

## Translation Strategy

It assumes to be translated to C source code.

### Declaration

`int` is a 64 bit number. It will be a `int64_t` type in C.
`dbl` is a double precision floating point number. It will be a `double` type in C.

```c
// var a;                         // a = 0
int64_t a = 0;

// var a:int;                     // a = 0
int64_t a = 0;

// var a = 10;                    // a = 10
int64_t a = 10;

// var a:int = 11;                // a = 11
int64_t a = 11;

// var a = 10, b:int = 20;        // a = 10, b = 20
int64_t a = 10;
int64_t b = 20;

// var a:int = 11, b = 21;        // a = 11, b = 21
int64_t a = 11;
int64_t b = 21;

// var a:int = 11, b:dbl = 21;    // a = 11, b = 21.0
int64_t a = 11;
double b = 21.0;

// var b = 2.0;                   // b = 2.0
double b = 2.0;
```

### Expression

Expression will be used as is.
If the type is mismatched between left and right hand side, a cast will be inserted automatically.

```c
// var a:int = 10;
// var b:int = 20;
// var c = a / b;    // `c` will be `double`.
int64_t a = 10;
int64_t b = 20;
double c = (double)a / (double)b;
```

### Flow Control

#### if

`if` is also used as is. A cast can be inserted into expression, but `if` is used as is.

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

`while` is also used as is. A cast can be inserted into expression, but `while` is used as is.

```javascript
while (expression) {
   ...
}
```

#### for

`for` is different only when the first expression is a declaration.
When it is a declaration, a declaration statement will be separated in a local scope like the following.

```c
// for (var a = 10, b = 2.0; expression; expression) {
//     ...
// }
{
    int64_t a = 10;
    double b = 2.0;
    for ( ; expression; expression) {
        ...
    }
}
```

### Function Definition

Basically a function is translated to the same style with translating a type and a name.

```c
// function:int functionName(a:int, b:int) {
//     ...
// }
int64_t i_functionName_ii(int64_t a, int64_t b)
{
    ...
}
```

# Conclusion

This is a rough design. When I notice a thing I should change in proceeding this project, it will be changed then.

I guess the strategy for translation will be changed a little when the closure is supported.
Because the closure will use a frame which has a local variable and will have to manage it.
But if all function has a frame, the performance issue will occur.
I will think about it.

