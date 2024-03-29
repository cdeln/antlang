# Antlang

An experimental statically typed Lisp.

## Introduction to the language
The current implementation compiles the source code directly into an executable runtime tree format (i.e. not bytecode).
The language compiler is called Antpile, and currently it takes a source file with a sequence of expressions, compiles them and then evaluates them directly like a script.

### Syntax

### Types and literals
Antlang supports all standard numeric types `bool`, `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32` and `f64`.
Literals are always annotated with their type. The literal `0` can thus be written as `(i32 0)` or `(f64 0)` depending on which type of `0` is used.

### Functions
Functions are defines as follows

    (function <function-name> <return-type> <parameters> <body>)

where parameters are a list of interleaved types and names

    (type name ...)

Function overloading is supported.

### Special forms

#### When
Antlang only supports one conditional built in `when` with the following syntax

    (when <branch>... <fallback>)

where `<branch>` is a pair of a `bool` expression and a value expression within square brackets

    [ <bool-expr>  <value-expr> ]

The `<fallback>` expression is used if no other branch is taken.

#### Let
Antlang supports let-expressions with the following syntax

    (let <binding>... <body)

where `<binding>` is a pair of a local variable name and value

    [ <name> value> ]


#### Structures
Antlang supports definition and construction of structures (but has no way to use them yet...).
A structure is defined by

    (structure <name> <fields>...)

where `<field>` is defined a sequence of types and field name.
For example, one might construct a tuple of `i32` as follows

    (structure i32-pair
      i32 first
      i32 second)

Semantically, defining a structure with `<name>` simultaneously defines a type with `<name>` and a constructor function with the same `<name>` with return type `<name>`.

### Example
As an example, we put together all syntax elements into a function counting all numbers between `0` and `n`

    (function sum i32 (i32 n)
      (when [(= n (i32 0)) (i32 0)]
        (let [tmp (sum (- n (i32 1)))]
          (+ n tmp))))

which can be evaluated as

    (sum (i32 1337))
    ;; => 894453

As you can see, the language is very verbose without any type inference.
More examples (including buggy ones) are given in the `snippets` folder.

## Dependencies

- One of [cmake](https://cmake.org/) or the slightly more esoteric (but pretty cool) [build2](https://build2.org) build systems.
- [doctest](https://github.com/doctest/doctest) for testing.
- [boost](https://www.boost.org)

You can install some of the dependencies on Ubuntu using the `install-deps.sh` script.

## Building
Using cmake

    ./scripts/configure
    ./scripts/build

or using build2

    b

## Runing the tests
If you use cmake, build and run the tests by

    ./scripts/build --target test
    ./out/tests/test

If you use build2, the tests are built by default. Also, build2 does an in source build, so you run the tests by

    ./tests/test

## Installing
Install the Antpile compiler and evaluator

    sudo ./scripts/install

Now you can compile Antlang programs using the `antpile` command.
