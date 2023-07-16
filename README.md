# Antlang

An experimental statically typed Lisp.

## Introduction to the language
The current implementation compiles the source code directly into an executable runtime tree format (i.e. not bytecode).
The language compiler is called Antpile, and currently it takes a source file with a sequence of expressions, compiles them and then evaluates them directly like a script.

### Syntax

### Functions
Functions are defines as follows

    (function <function-name> <return-type> <parameters> <body>)

where parameters are a list of interleaved types and names

    (type name ...)

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
