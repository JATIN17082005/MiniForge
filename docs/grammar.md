# MiniForge Language Grammar & Precedence Specification

This document defines the formal grammar, operator precedence, and associativity for the MiniForge programming language.

---

## 1. Lexical Elements

### 1.1 Keywords
The following identifiers are reserved keywords:
```text
let      if       else     while    print
```

### 1.2 Literals and Identifiers
- **Identifiers**: Begin with an ASCII letter or underscore (`[a-zA-Z_]`), followed by zero or more alphanumeric characters or underscores (`[a-zA-Z0-9_]*`).
- **Integer Literals**: Unsigned sequence of decimal digits (`[0-9]+`), parsed into 64-bit signed integers (`int64_t`).

### 1.3 Operators & Punctuation
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Assignment: `=`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Grouping: `(`, `)`
- Blocks: `{`, `}`
- Delimiters: `;`, `,`

### 1.4 Comments
- Single-line comments begin with `//` and continue until the end of the line or file.
- Comments and whitespace (spaces, tabs, carriage returns, newlines) are treated as token separators and ignored.

---

## 2. Operator Precedence and Associativity

MiniForge expressions follow standard C/C++ precedence rules, listed here from lowest to highest:

| Precedence | Operator | Description | Associativity |
|---|---|---|---|
| **1 (Lowest)** | `==`, `!=` | Equality and inequality | Left-to-right |
| **2** | `<`, `<=`, `>`, `>=` | Relational comparisons | Left-to-right |
| **3** | `+`, `-` | Addition and subtraction | Left-to-right |
| **4** | `*`, `/`, `%` | Multiplication, division, modulo | Left-to-right |
| **5 (Highest)**| Prefix `-` | Unary negation | Right-to-left |

Parentheses `( ... )` can be used to override default precedence.

---

## 3. Formal Syntax (EBNF)

```ebnf
Program         ::= Statement* EOF ;

Statement       ::= VarDeclStmt
                  | AssignStmt
                  | IfStmt
                  | WhileStmt
                  | PrintStmt
                  | BlockStmt ;

VarDeclStmt     ::= "let" IDENTIFIER "=" Expression ";" ;

AssignStmt      ::= IDENTIFIER "=" Expression ";" ;

IfStmt          ::= "if" "(" Expression ")" BlockStmt ( "else" BlockStmt )? ;

WhileStmt       ::= "while" "(" Expression ")" BlockStmt ;

PrintStmt       ::= "print" "(" Expression ")" ";" ;

BlockStmt       ::= "{" Statement* "}" ;

Expression      ::= EqualityExpr ;

EqualityExpr    ::= RelationalExpr ( ( "==" | "!=" ) RelationalExpr )* ;

RelationalExpr  ::= AdditiveExpr ( ( "<" | "<=" | ">" | ">=" ) AdditiveExpr )* ;

AdditiveExpr    ::= MultiplicativeExpr ( ( "+" | "-" ) MultiplicativeExpr )* ;

Multiplicative  ::= UnaryExpr ( ( "*" | "/" | "%" ) UnaryExpr )* ;

UnaryExpr       ::= "-" UnaryExpr
                  | PrimaryExpr ;

PrimaryExpr     ::= INTEGER_LITERAL
                  | IDENTIFIER
                  | "(" Expression ")" ;
```

---

## 4. Disambiguation Rules

1. **Dangling Else**:
   An `else` clause binds to the nearest open `if` statement. MiniForge enforces curly braces `{ ... }` for `if` and `else` bodies, eliminating classical dangling-else ambiguity at the syntax level.

2. **Statement Termination**:
   All simple statements (`let`, assignment, `print`) must terminate with a semicolon `;`. Block statements (`{ ... }`, `if`, `while`) do not require a trailing semicolon.
