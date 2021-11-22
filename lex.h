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
#ifndef _LEX_H
#define _LEX_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

enum Symbol // Creates an enumerated type Symbol to hold all tokens
{
    begint = 0,
    modulet = 1,
    constantt = 2,
    proceduret = 3,
    ist = 4,
    ift = 5,
    thent = 6,
    elset = 7,
    elsift = 8,
    whilet = 9,
    loopt = 10,
    floatt = 11, //realt?
    integert = 12,
    chart = 13,
    gett = 14,
    putt = 15,
    putlnt = 16,
    intok = 17,
    outt = 18,
    inoutt = 19,
    nott = 20,
    endt = 21,
    relopt = 22,
    addopt = 23,
    mulopt = 24,
    signopt = 25,
    assignopt = 26,
    lparent = 27,
    rparent = 28,
    commat = 29,
    colont = 30,
    semit = 31,
    periodt = 32,
    literalt = 33,
    numt = 34,
    idt = 35,
    eoft = 36,
    unknownt = 37
};

typedef string resword[endt - begint + 1]; // Creates a user defined string type called resword that references the reserved words from the Symbol structure

extern string symbols[38];
extern resword reswords;
extern Symbol Token;
extern string Lexeme;
extern int LineNo;
extern int Value;
extern float ValueR;
extern char ch;
extern string Literal;
extern int Depth;
extern char* file;

class Lexical // Creates a class called Lexical that holds all the lexical analysis functions
{
public:
    ifstream infile;

    Lexical(char* filename);

    ~Lexical();

    /******************************************************************************
    *** FUNCTION: Lexical::GetNextToken()                                       ***
    *******************************************************************************
    *** DESCRIPTION : Gets the next token of a line of code and processes it.   ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void GetNextToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessToken()                                       ***
    *******************************************************************************
    *** DESCRIPTION : Processes the different types of tokens found.            ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessWordToken()                                   ***
    *******************************************************************************
    *** DESCRIPTION : Processes different word tokens such as mulopt and idt.   ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessWordToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessNumToken()                                    ***
    *******************************************************************************
    *** DESCRIPTION : Processes the num token for floats and integers.          ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessNumToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessCommentToken()                                ***
    *******************************************************************************
    *** DESCRIPTION : Finds comments and skips/ignores them until a new line.   ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessCommentToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessDoubleToken()                                 ***
    *******************************************************************************
    *** DESCRIPTION : Processes double tokens such as relopt and assignopt.     ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessDoubleToken();

    /******************************************************************************
    *** FUNCTION: Lexical::ProcessSingleToken()                                 ***
    *******************************************************************************
    *** DESCRIPTION : Processes all single tokens such as relopt, addopt, mulopt,**
    *** lparent, rparent, commat, colont, semit, periodt, and literalt.         ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcessSingleToken();

    /******************************************************************************
    *** FUNCTION: Lexical::LexemeResize()                                       ***
    *******************************************************************************
    *** DESCRIPTION : Resizes the Lexeme to be however many characters it       ***
    *** actually is.                                                            ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void LexemeResize();

    /******************************************************************************
    *** FUNCTION: Lexical::ToUpperString(string str)                            ***
    *******************************************************************************
    *** DESCRIPTION : Reads in a string and converts all lowercase letters      ***
    *** to uppercase letters.                                                   ***
    *** INPUT ARGS : str                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ToUpperString(string str);
};

#endif 