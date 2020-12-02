# Kinx-Static

This is a trial project using MIR. The target is a statically typed language which has a similar syntax with Kinx.

* Kinx - https://github.com/Kray-G/kinx
* MIR - https://github.com/vnmakarov/mir

And also I hope this project is to be a simple guideline of how to create your own language easily.

# Introduction

## Motivation

### Do you want to create your own language?

I have been very interested in [MIR](https://github.com/vnmakarov/mir) since it was a first look.
So I try to create a sample language based on MIR.
If this is successful, I believe you can define your own language as you like.
For that purpose, I will try to record how to create a frontend of a language and to make it a frontend of MIR.

MIR has a C compiler and an on-the-fly executor of MIR.
Therefore the language can use a C code instead of MIR directly.
This is a great benefit of MIR.

This Kinx-Static example will also use a C code translation.

## Objective

* To use MIR.
* To create the Kinx-Static language.
* To record how to create a language.
* For you to be able to create your own language.

# Features

T.B.D.

# How To Create Your Own Language

Let's see how to create your own language through the example of how to create Kinx-Static language.

### [1. BNF definition](history/1)

You will be able to define your language as BNF style.
This time it is written in the file format of yacc/bison.

* introduced `kinxstatic.y` and `kmyacc.exe`/`kmyacc`.

### 2. Building AST

You will be able to write a lexer.
And you will be able to build AST, which means Abstract Syntax Tree, by combining between a lexer and a parser.
AST is a tree and it will be generated as a tree of `ast_node` object.

* introduced `lexer.c`.
* introduced `ast_node.c` and `ast_node.h`.
* modified `kinxstatic.y`.

### 3. Generating Code

MIR supports an execution directly from C source code.
This means, only you have to do is to translate AST to pure C source code.

* introduced `string.c` and `string.h`.
* introduced `gencode.c`.

### 4. Integration with MIR

To execute it, let's pass the generated C source code to MIR compiler and executor.

* introduced `execute.c`.
* introduced MIR backend.

### 5. More Features

I will continue to update Kinx-Static with ...
