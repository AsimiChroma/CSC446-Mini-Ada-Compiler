# Mini-Ada-Compiler
A mini-Ada compiler that parses through a small set of Ada grammar rules of an Ada file and generates a Three Address Code (TAC), which is then translated into 8086 assembly language.
Some test files are included so you can try it out.


NOTE: Before running the program, make sure to remove the .asm and .TAC files. Your code will generate the .asm and .TAC files, so you can double check the contents with my files.

To run the code, you can use clang 7 or higher to compile and run the code by typing this on your terminal:

    clang++-7 -pthread -std=c++17 -o main codegen.cpp lex.cpp main.cpp rdparser.cpp symtable.cpp
 
 Then you can type in the file you want to run. For example for test file 51, you can type:
 
    ./main t51.ada
 
 The .ada file will be fed in and compiled and a TAC file will be generated, and a .asm file will also be made.
 
 
