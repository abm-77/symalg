# SymAlg Calculator
This is a symbolic algebra calculator written in "C-like" C++. 

Q: What is "C-like" C++? 

A: Essentially I tried to limit the number of C++ features I used. The main features I took advantage of in this program were maps (which I might implement from scratch in the future) and lambdas (as closures).

Currently the project is set to build on windows, but with a few minor adjustments (switching the underlying arena allocators to use other allocator instead of VirtualAlloc), it can be ported to most other operating systems. An executable file is included in the bin folder.

# Build Instructions:
To build from source, all you have to do is build the main.cpp file (all other header and implementation files are included in the main.cpp file) with your choice of compiler. MSVC's cl was used in the bat files included, but those should serve as a guideline of how to write a build script using clang++, g++, etc. 

# Usage Instructions:
Welcome to the SymAlg Calculator! This solver with evaluate and simplify basic algebraic expressions (+, -, /, *, ^).
Any expression entered must be contained in parentheses (i.e (10 + 5), ((1 + 2) / (3 - 1)), etc.).

The solver has basic variable support.

Variables are considered length-1 strings and are case sensitive.
The syntax to set the value of a variable is: (VAR = VALUE). Where VAR is the name of the variable, and VALUE is any valid symbolic expression.

Type 'quit' to exit.

Type 'clear' to clear all stored variables.