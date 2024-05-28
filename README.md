# DrewnoMarsCompiler

DrewnoMarsCompiler is a custom compiler written in C++ for a simplified programming language, designed to demonstrate basic compiler construction and language design principles. This README provides an overview of the compiler, including its features, installation, usage, and a detailed explanation of the AST (Abstract Syntax Tree) implementation.

## Features

- **Variable Declarations**: Support for integer variables.
- **Control Flow**: Implementation of `while` loops and `if-else` conditionals.
- **Output Operations**: Simple output commands for displaying variables and strings.
- **Type Analysis**: Ensures type correctness of expressions and statements.
- **Symbol Table Management**: Maintains scope and binding information for variables and functions.
- **Intermediate Representation (IR)**: Generates intermediate code for further compilation stages.

## Installation

To install and run the DrewnoMarsCompiler, follow these steps:

1. **Clone the Repository**: Clone the repository from GitHub.
   ```sh
   git clone https://github.com/porterfurlongku/DrewnoMarsCompiler.git
   cd DrewnoMarsCompiler

2. **Compile the Compiler**: Use a C compiler to compile the DrewnoMarsCompiler source code.
   ```sh
   gcc -o DrewnoMarsCompiler DrewnoMarsCompiler.c
3. **Run the Compiler**:  Use the compiled compiler to execute a DrewnoMarsCompiler program.
   ```sh
   ./DrewnoMarsCompiler example.dm

## Usage

To use the DrewnoMarsCompiler, write your programs in a text file with the `.drewno` extension and run the compiler as shown above. The compiler will parse and execute the program, displaying the output as specified by the `give` commands in the code.


## Abstract Syntax Tree (AST) Overview

The AST is a crucial component of the DrewnoMarsCompiler, representing the hierarchical structure of the source code. The `ast.hpp` file defines various classes to construct and manipulate the AST.

### Key Classes

- **ASTNode**: Base class for all AST nodes.
- **ProgramNode**: Represents the entire program, containing global declarations.
- **ExpNode**: Base class for all expression nodes.
- **LocNode**: Base class for location nodes, representing variables or storage locations.
- **StmtNode**: Base class for all statement nodes.
- **DeclNode**: Base class for all declaration nodes.
- **TypeNode**: Represents types in the language.
- **IDNode**: Represents identifiers (variable names).

### Expression Nodes

- **BinaryExpNode**: Base class for binary expressions (e.g., addition, subtraction).
- **UnaryExpNode**: Base class for unary expressions (e.g., negation).
- **IntLitNode**: Represents integer literals.
- **StrLitNode**: Represents string literals.
- **TrueNode**: Represents the boolean literal `true`.
- **FalseNode**: Represents the boolean literal `false`.
- **CallExpNode**: Represents function calls.

### Statement Nodes

- **AssignStmtNode**: Represents assignment statements.
- **GiveStmtNode**: Represents output statements.
- **IfStmtNode**: Represents `if` statements.
- **IfElseStmtNode**: Represents `if-else` statements.
- **WhileStmtNode**: Represents `while` loops.
- **ReturnStmtNode**: Represents return statements.

### Declaration Nodes

- **VarDeclNode**: Represents variable declarations.
- **FormalDeclNode**: Represents function parameter declarations.
- **FnDeclNode**: Represents function declarations.

### Type Nodes

- **VoidTypeNode**: Represents the `void` type.
- **IntTypeNode**: Represents the `int` type.
- **BoolTypeNode**: Represents the `bool` type.
- **PerfectTypeNode**: Represents a custom type with sub-types.

## Example Code

Below is an example program written in the DrewnoMarsCompiler language:

```drewno
main: () void{
    while(false){
        a: int = 1;
        give a;
        give "\n";
    }
    if(true){
        a: int = 1;
        give a;
        give "\n";
    }
    if(false){
        a: int = 1;
    } else {
        a: int = 1;
        a++;
        give a;
        give "\n";
        a--;
        give a;
    }
}


