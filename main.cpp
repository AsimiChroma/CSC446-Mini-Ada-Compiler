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
#include "rdparser.h"
#include "codegen.h"
#include <iomanip>

using namespace std;

extern Symbol Token;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
    }
    else
    {
        //cout << "Note: -- Class 0 = Constant -- Class 1 = Variable -- Class 2 = Procedure --" << endl;
        Parser MyParse(argv[1]);
        MyParse.GetNextToken();
        MyParse.Prog();
        //cout << "------------------------" << endl;
        //cout << "Current Depth: " << Depth << endl;
        //MyParse.writeTable(0);
        if (Token != eoft)
        {
            cout << ("ERROR! - unused tokens") << endl;
            exit(1);
        }
          
        // Prints out code to asm file
        CodeGen MyCode;
        MyCode.dataSegHead = MyParse.dataSegHead;
        MyCode.procVarHead = MyParse.procVarHead;
        MyCode.asmTemplate();

        cout << "-- Finished! --" << endl;
    }
    
    return 0;
} // End of main.