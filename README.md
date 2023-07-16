# Antlang

A statically typed LISP-like language.

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

## Run a Antlang file as a script
If you built using cmake, run it like

    ./out/programs/antpile snippets/fibonacci.ant

If you built it using build2, run it like

    ./programs/antpile snippets/fibonacci.ant
