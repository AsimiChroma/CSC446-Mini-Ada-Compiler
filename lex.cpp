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
#include <iostream>
#include "lex.h"
#include <iomanip>
#include <cstring>

using namespace std;

// Create an array of reswords and stores all the actual reserved word values
resword reswords =
{ "BEGIN", "MODULE", "CONSTANT", "PROCEDURE", "IS", "IF", "THEN", "ELSE", "ELSIF", "WHILE", "LOOP", "FLOAT","INTEGER", "CHAR", "GET", "PUT", "PUTLN", "IN", "OUT", "INOUT", "NOT", "END" };

// Creates an array of symbols which store all the actual token values
string symbols[38] = { "begint", "modulet", "constantt", "proceduret", "ist", "ift", "thent", "elset", "elsift", "whilet", "loopt", "floatt", "integert", "chart", "gett", "putt", "putlnt","intok", "outt", "inoutt", "nott", "endt", "relopt", "addopt", "mulopt", "signopt", "assignopt", "lparent", "rparent", "commat", "colont", "semit", "periodt", "literalt", "numt", "idt", "eoft", "unknownt" };

// Global Variable declarations
Symbol Token = unknownt;
string Lexeme(17, '\0');
int LineNo = 1;
int Value;
float ValueR;
char ch = ' ';
string Literal;
int Depth = 0;
char* file;

Lexical::Lexical(char* filename) // Constructor for opening file
{
    infile.open(filename);
    file = filename;
    file[strlen(file) - 3] = 'T';
    file[strlen(file) - 2] = 'A';
    file[strlen(file) - 1] = 'C';
}

Lexical::~Lexical() // Destructor for closing file
{
    infile.close();
}

/******************************************************************************
*** FUNCTION: Lexical::GetNextToken()                                       ***
*******************************************************************************
*** DESCRIPTION : Gets the next token of a line of code and processes it.   ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::GetNextToken()
{
    Literal.clear();
    Lexeme.clear();
    Lexeme.resize(17, '\0');

    // Check if ch is whitespace and skip it
    while (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\f' || ch == '\v' || ch == '\r')
    {
        if (infile.eof())
        {
            Token = eoft;
            return;
        }

        if (ch == '\n')
        {
            LineNo++;
        }
        infile.get(ch);
    }

    if (!infile.eof())
    {
        ProcessToken();
    }
    else
    {
        Token = eoft;
    }
    //cout << "Lexeme: " << Lexeme << endl;
}

/******************************************************************************
*** FUNCTION: Lexical::ProcessToken()                                       ***
*******************************************************************************
*** DESCRIPTION : Processes the different types of tokens found.            ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::ProcessToken()
{
    Lexeme[0] = ch;
    infile.get(ch);

    if (('A' <= Lexeme[0] && Lexeme[0] <= 'Z') || ('a' <= Lexeme[0] && Lexeme[0] <= 'z'))
    {
        ProcessWordToken();
    }
    else if ('0' <= Lexeme[0] && Lexeme[0] <= '9')
    {
        ProcessNumToken();
        LexemeResize();
    }
    else if (Lexeme[0] == '-' && ch == '-')
    {
        ProcessCommentToken();
        //cout << Lexeme << endl;
        GetNextToken(); // added **********
    }
    else if ((Lexeme[0] == ':' || Lexeme[0] == '/' || Lexeme[0] == '<' || Lexeme[0] == '>') && (ch == '='))
    {
        ProcessDoubleToken();
        LexemeResize();
    }
    else
    {
        ProcessSingleToken();
        LexemeResize();
    }
}

/******************************************************************************
*** FUNCTION: Lexical::ProcessWordToken()                                   ***
*******************************************************************************
*** DESCRIPTION : Processes different word tokens such as mulopt and idt.   ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::ProcessWordToken()
{
    // Was ReadRest(), now integrated into ProcessWordToken()//
    int i = 1;
    while ((('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || (ch == '_')) && !infile.eof())
    {

        if (i > Lexeme.size() - 1)
        {
            Token = unknownt;
            Literal = "ERROR: lexeme is more than 17 characters on line " + to_string(LineNo);
            //cout << "--ERROR: lexeme is more than 17 characters on line " << LineNo << ':' << endl;
            while ((('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || (ch == '_')) && !infile.eof())
            {
                infile.get(ch);
            }
            ToUpperString(Lexeme);
            return;
        }
        Lexeme[i] = ch;
        infile.get(ch);
        i++;
    }

    ToUpperString(Lexeme); // Converts all characters to uppercase for the lexeme
    LexemeResize(); // Finds the new size of the lexeme.

    Symbol sym;
    for (sym = begint; sym <= endt; sym = Symbol(sym + 1))
    {
        if (reswords[sym] == Lexeme)
        {
            Token = sym;
            return;
        }
    }
    if (Lexeme == "MOD" || Lexeme == "REM" || Lexeme == "AND")
    {
        Token = mulopt;
    }
    else if (Lexeme == "OR")
    {
        Token = addopt;
    }
    else
    {
        Token = idt;
    }
}

/******************************************************************************
*** FUNCTION: Lexical::ProcessNumToken()                                    ***
*******************************************************************************
*** DESCRIPTION : Processes the num token for floats and integers.          ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::ProcessNumToken()
{
    Token = numt;

    if (ch == '.')
    {
        Lexeme[1] = ch;
        infile.get(ch);
        if ('0' <= ch && ch <= '9')
        {
            int i = 2;
            while (('0' <= ch && ch <= '9' && !infile.eof()))
            {
                Lexeme[i] = ch;
                infile.get(ch);
                i++;
            }
        }
        else
        {
            //cout << "--ERROR: lexeme should be a valid float or integer on line " << LineNo << ':' << endl;
            Literal = "ERROR: lexeme should be a valid float or integer on line " + to_string(LineNo);
            Token = unknownt;
            return;
        }
    }
    else
    {
        int i = 1;
        while (('0' <= ch && ch <= '9' && !infile.eof()))
        {
            Lexeme[i] = ch;
            infile.get(ch);
            i++;
        }

        if (ch == '.')
        {
            Lexeme[i] = ch;
            infile.get(ch);
            i++;

            if ('0' <= ch && ch <= '9')
            {
                while ('0' <= ch && ch <= '9')
                {
                    Lexeme[i] = ch;
                    infile.get(ch);
                    i++;
                }
            }
            else
            {
                Token = unknownt;
                //cout << "--ERROR: lexeme should be a valid float or integer on line " << LineNo << ':' << endl;
                Literal = "ERROR: lexeme should be a valid float or integer on line " + to_string(LineNo);
                return;
            }
        }
    }

    if (Lexeme.find('.') == -1) // If there isn't a . then set value to be integer value
    {
        Value = stof(Lexeme);
    }
    else
    {
        ValueR = stof(Lexeme); // Else if there is a . then set valueR to be float value
    }
}

/******************************************************************************
*** FUNCTION: Lexical::ProcessCommentToken()                                ***
*******************************************************************************
*** DESCRIPTION : Finds comments and skips/ignores them until a new line.   ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::ProcessCommentToken()
{
    //cout << "Comment Lexeme: " << Lexeme << endl;
    Lexeme[1] = ch;
    while ((ch != '\n' && !infile.eof()))
    {
        infile.get(ch);
        //cout << ch << endl;
    }
}

/******************************************************************************
*** FUNCTION: Lexical::ProcessDoubleToken()                                 ***
*******************************************************************************
*** DESCRIPTION : Processes double tokens such as relopt and assignopt.     ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Lexical::ProcessDoubleToken()
{
    Lexeme[1] = ch;
    if ((Lexeme[0] == '/' && ch == '=') || (Lexeme[0] == '<' && ch == '=') || (Lexeme[0] == '>' && ch == '='))
    {
        Token = relopt;
        return;
    }
    else
    {
        Token = assignopt;
    }
    infile.get(ch);
}

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
void Lexical::ProcessSingleToken()
{
    if (Lexeme[0] == '=' || Lexeme[0] == '<' || Lexeme[0] == '>')
    {
        Token = relopt;
        return;
    }
    else if (Lexeme[0] == '+' || Lexeme[0] == '-')
    {
        Token = addopt;
        return;
    }
    else if (Lexeme[0] == '*' || Lexeme[0] == '/')
    {
        Token = mulopt;
        return;
    }
    else if (Lexeme[0] == '(')
    {
        Token = lparent;
        return;
    }
    else if (Lexeme[0] == ')')
    {
        Token = rparent;
        return;
    }
    else if (Lexeme[0] == ',')
    {
        Token = commat;
        return;
    }
    else if (Lexeme[0] == ':')
    {
        Token = colont;
        return;
    }
    else if (Lexeme[0] == ';')
    {
        Token = semit;
        return;
    }
    else if (Lexeme[0] == '.')
    {
        Token = periodt;
        return;
    }
    else if (Lexeme[0] == '"')
    {
        // hits a 17 characters length -> Lexeme, or if not 17 char, hit until " -> Lexeme
        // hits " -> Literal
        Literal = Lexeme.substr(0, 1);

        int i = 1;
        while (ch != '"')
        {
            if ((ch == '\n' || infile.eof()))
            {
                Literal = "ERROR: No end quote in line " + to_string(LineNo);
                Token = unknownt;
                return;
            }

            if (i < Lexeme.size())
            {
                Lexeme[i] = ch;
            }
            Literal.resize(Literal.size() + 1);
            Literal[i] = ch;
            infile.get(ch);
            i++;
        }

        if (i < Lexeme.size())
        {
            Lexeme[i] = ch; //ch == '"'
        }

        Literal.resize(Literal.size() + 1);
        Literal[i] = ch;
        Token = literalt;

        infile.get(ch);
        return;
    }
    else
        Token = unknownt;
}


// ---------- String handler Portion ----------//


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
void Lexical::LexemeResize()
{
    int i = 0;
    while (Lexeme[i] != '\0')
    {
        i++;
    }
    Lexeme.resize(i);
}

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
void Lexical::ToUpperString(string str)
{
    for (int i = 0; Lexeme[i] != '\0'; i++)
    {
        if (Lexeme[i] >= 'a' && Lexeme[i] <= 'z')   // checks for lowercase characters
            Lexeme[i] = Lexeme[i] - 32;        // converting lowercase to uppercase
    }
}