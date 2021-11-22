/*****************************************************************************
*** NAME        : Anqi Zhang                                               ***
*** CLASS       : CSC 446                                                  ***
*** ASSIGNMENT  : 8 Semi-Ada Compiler                                      ***
*** DUE DATE    : Apr 30, 2021                                             ***
*** INSTRUCTOR  : Hamer                                                    ***
******************************************************************************
*** DESCRIPTION : RD Parser with Three Address Code                        ***
*** This RD Parser module will read a source program and check if          ***
*** it is a legal program. It will output error messages if illegal.       ***
*** Type, Size, and Offset will be displayed if needed.                    ***
*****************************************************************************/
#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

extern char * asmFile;

class CodeGen : public SymbolTable // Creates a class called CodeGen that holds all the code generation functions and inherits from SymbolTable
{
    public:
    ifstream infile;
    using SymbolTable::SymbolTable;
    CodeGen();

    ~CodeGen();

    void asmTemplate();
    void emit(string firstCol, string secondCol, string thirdCol);
    void dataSeg();
    void TACparser();
    string getFirstWord(string line);
    string getSecondWord(string line);
    string getThirdWord(string line);
    string getFourthWord(string line);
    string getFifthWord(string line);
    void CopyStat(string dest, string source);
    int getTotalWords(string line);    
    void AddStat(string add1, string add2, string sum);
    void MultiStat(string mult1, string mult2, string product);
    string formatOperand(string str);

};















#endif 
