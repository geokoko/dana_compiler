# Dana Language Reference

Dana is a procedural programming language designed for educational purposes. It features:

- Strong static typing with two basic types: `int` and `byte`
- Procedures and functions with pass-by-value and pass-by-reference semantics
- Arrays with compile-time known dimensions
- Layout-sensitive syntax (indentation-based blocks)
- Forward declarations for recursive definitions

## Lexical Elements

### Keywords

```
and     as      begin   break   byte    continue
decl    def     elif    else    end     exit
false   if      int     is      loop    not
or      ref     return  skip    true    var
```

### Operators

| Category    | Operators                   |
|-------------|-----------------------------|
| Arithmetic  | `+` `-` `*` `/` `%`         |
| Bitwise     | `&` `\|` `!`                |
| Comparison  | `=` `<>` `<` `>` `<=` `>=`  |
| Logical     | `and` `or` `not`            |
| Assignment  | `:=`                        |

### Literals

- **Integers**: Decimal digits, e.g., `42`, `0`, `123`
- **Characters**: Single-quoted, e.g., `'a'`, `'\n'`, `'\x41'`
- **Strings**: Double-quoted, e.g., `"Hello world!\n"`
- **Booleans**: `true` (value 1), `false` (value 0)

### Comments

Single-line comments start with `--` and extend to the end of the line:

```dana
-- This is a comment
var x is int  -- This is also a comment
```

Multi-line (nested) comments use `(*` and `*)`:

```dana
(* This is a 
   multi-line comment 
   (* with nesting support *) 
*)
```

## Types

### Basic Types

| Type   | Description                        | Size    |
|--------|------------------------------------|---------|
| `int`  | Signed integer                     | 32 bits |
| `byte` | Unsigned byte (0-255)              | 8 bits  |

### Arrays

Arrays have fixed dimensions known at compile time:

```dana
var arr is int[10]           -- 1D array of 10 integers
var matrix is byte[3][4]     -- 2D array (3×4)
```

For function parameters, the first dimension can be unspecified:

```dana
def printArray: arr as int [], n as int
    -- arr's first dimension is unknown, passed as pointer
```

## Declarations

### Variable Declarations

Variables are declared with `var`:

```dana
var x is int
var a b c is int          -- Multiple variables of same type
var buffer is byte[256]
```

### Function Definitions

Functions return a value and are defined with `def`:

```dana
def factorial is int: n as int
    if n <= 1: return: 1
    else: return: n * factorial(n - 1)
```

### Procedure Definitions

Procedures do not return a value:

```dana
def printHello
    writeString: "Hello!\n"
```

### Forward Declarations

Use `decl` for forward declarations (required for mutual recursion):

```dana
decl isEven is byte: n as int

def isOdd is byte: n as int
    if n = 0: return: false
    else: return: isEven(n - 1)

def isEven is byte: n as int
    if n = 0: return: true
    else: return: isOdd(n - 1)
```

## Parameters

### Pass by Value

Default for scalar types:

```dana
def double is int: x as int
    return: x * 2
```

### Pass by Reference

Use `ref` keyword for scalars, or arrays (always by reference):

```dana
def swap: x y as ref int
    var t is int
    t := x
    x := y
    y := t

def sumArray is int: arr as int [], n as int
    -- Arrays are always passed by reference
```

## Statements

### Assignment

```dana
x := 42
arr[i] := arr[i] + 1
```

### Procedure Calls

```dana
writeString: "Hello"
swap: a, b
writeInteger: x + y
```

### Control Flow

#### If Statement

```dana
if x > 0:
    writeString: "positive"
elif x < 0:
    writeString: "negative"
else:
    writeString: "zero"
```

#### Loop Statement

Dana has infinite loops with explicit `break` and `continue`:

```dana
loop:
    if i >= n: break
    writeInteger: arr[i]
    i := i + 1
```

Loops can have labels:

```dana
loop outer:
    loop inner:
        if done: break: outer  -- Break from outer loop
        if skip: continue: inner
```

### Return and Exit

```dana
def compute is int: x as int
    if x < 0: return: 0       -- Return from function with value
    -- ...
    return: result

def process: x as int
    if x < 0: exit            -- Exit from procedure (no value)
    -- ...
```

### Skip Statement

The `skip` statement does nothing (useful for empty blocks):

```dana
if condition:
    skip
else:
    doSomething
```

## Expressions

### Arithmetic Expressions

```dana
x + y        -- Addition
x - y        -- Subtraction
x * y        -- Multiplication
x / y        -- Integer division
x % y        -- Modulo (remainder)
+x           -- Unary plus
-x           -- Unary minus
```

### Bitwise Expressions (byte only)

```dana
a & b        -- Bitwise AND
a | b        -- Bitwise OR
!a           -- Bitwise NOT
```

### Function Calls

```dana
factorial(5)
readInteger()
strlen("hello")
```

## Conditions

Conditions are a separate syntactic category from expressions. They appear in `if` and `elif` clauses.

### Relational Conditions

```dana
x = y        -- Equal
x <> y       -- Not equal
x < y        -- Less than
x > y        -- Greater than
x <= y       -- Less than or equal
x >= y       -- Greater than or equal
```

### Logical Conditions

```dana
cond1 and cond2     -- Logical AND
cond1 or cond2      -- Logical OR
not cond            -- Logical NOT
(cond)              -- Parenthesized condition
```

**Important**: Bare expressions cannot be used as conditions. You must use a comparison:

```dana
-- INCORRECT:
if x:              -- ERROR: bare expression

-- CORRECT:
if x <> 0:         -- Use comparison
if x = true:       -- Compare to boolean
```

## Built-in Functions

### Output

| Function        | Description                           |
|-----------------|---------------------------------------|
| `writeInteger`  | Print an integer                      |
| `writeByte`     | Print a byte as integer               |
| `writeChar`     | Print a character                     |
| `writeString`   | Print a string                        |

### Input

| Function       | Description                            |
|----------------|----------------------------------------|
| `readInteger`  | Read an integer from stdin             |
| `readByte`     | Read a byte from stdin                 |
| `readChar`     | Read a character from stdin            |
| `readString`   | Read a string into buffer              |

### Type Conversion

| Function   | Description                              |
|------------|------------------------------------------|
| `extend`   | Convert byte to int (sign extension)     |
| `shrink`   | Convert int to byte (truncation)         |

### String Operations

| Function   | Description                              |
|------------|------------------------------------------|
| `strlen`   | Get string length                        |
| `strcmp`   | Compare two strings                      |
| `strcpy`   | Copy string to destination               |
| `strcat`   | Concatenate strings                      |

## Program Structure

A Dana program consists of a single top-level procedure definition. This procedure:
- Must have no parameters
- Must not return a value
- Serves as the program entry point

```dana
def main
    -- local definitions (variables, functions, procedures)
    var x is int
    
    def helper is int: n as int
        return: n * 2
    
    -- statements
    x := readInteger()
    writeInteger: helper(x)
    writeString: "\n"
```

## Layout Rules

Dana uses layout-sensitive syntax similar to Python:

1. Blocks are delimited by indentation
2. Statements at the same indentation level are siblings
3. Guide keywords (`def`, `if`, `elif`, `else`, `loop`) start new blocks
4. The compiler automatically infers block boundaries

Alternative explicit block syntax with `begin`/`end`:

```dana
def factorial is int: n as int
begin
    if n <= 1: begin return: 1 end
    else: begin return: n * factorial(n - 1) end
end
```

## Operator Precedence

From lowest to highest:

| Precedence | Operators           | Associativity |
|------------|---------------------|---------------|
| 1          | `or`                | Left          |
| 2          | `and`               | Left          |
| 3          | `\|`                | Left          |
| 4          | `&`                 | Left          |
| 5          | `+` `-`             | Left          |
| 6          | `*` `/` `%`         | Left          |
| 7          | `+` `-` `!` `not`   | Prefix        |

Note: Comparison operators (`=`, `<>`, `<`, `>`, `<=`, `>=`) are not part of expression precedence as they only appear in condition contexts.
