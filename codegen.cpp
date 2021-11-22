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
#include "rdparser.h"
#include "lex.h"
#include <iomanip>
#include "symtable.h"
#include "codegen.h"
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

char * asmFile;

CodeGen::CodeGen() // Constructor for opening file
{
    infile.open(file);
    asmFile = file;
    asmFile[strlen(asmFile) - 3] = 'a';
    asmFile[strlen(asmFile) - 2] = 's';
    asmFile[strlen(asmFile) - 1] = 'm';
}

CodeGen::~CodeGen() // Destructor for closing file
{
    infile.close();
}

/******************************************************************************
*** FUNCTION: asmTemplate()                                                 ***
*******************************************************************************
*** DESCRIPTION : creates and prints asm template                           ***
*** INPUT ARGS : code                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::asmTemplate()
{
    emit("", ".model small", ""); //cout << left << setw(12) << "" << setw(0) << ".model small" << endl;
    emit("", ".586", ""); //cout << left << setw(12) << "" << setw(0) << ".586" << endl;
    emit("", ".stack 100h", ""); //cout << left << setw(12) << "" << setw(0) << ".stack 100h" << endl;
    emit("", ".data", ""); //cout << left << setw(12) << "" << setw(0) << ".data" << endl;

    dataSeg();

    emit("", ".code", ""); //cout << left << setw(12) << "" << setw(0) << ".code" << endl;
    emit("", "include io.asm", ""); //cout << left << setw(12) << "" << setw(0) << "include io.asm" << endl;

    emit("start", "proc", "");
    emit("", "mov", "ax,@data");
    emit("", "mov", "ds,ax");
    emit("", "call ", mainProcVar()->variable);
    emit("", "mov", "ax,04ch");
    emit("", "int", "21h");
    emit("start", "endp", "");
    emit("","","");

    TACparser();
}

/******************************************************************************
*** FUNCTION: dataSeg()                                                     ***
*******************************************************************************
*** DESCRIPTION : saves depth 1 variables to print out                      ***
*** INPUT ARGS : code                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::dataSeg()
{
    dataSegPtr ptr = dataSegHead;
    while (ptr != NULL)
    {
        if(ptr->value == "")
        {
            emit(ptr->variable, "DW", "?"); //cout << left << setw(12) << ptr->variable << setw(8) << "DW" << setw(0) << "?" << endl;
        }
        else
        {
            emit(ptr->variable, "DB", ptr->value + ",\"$\"");
        }
        ptr = ptr->next;
    }
}

/******************************************************************************
*** FUNCTION: emit(string code)                                             ***
*******************************************************************************
*** DESCRIPTION : prints the asm into a new file                            ***
*** INPUT ARGS : code                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::emit(string firstCol, string secondCol, string thirdCol)
{
    static ofstream outFile;
    if (!outFile.is_open())
    {
        outFile.open(asmFile);
    }
    outFile << left << setw(12) << firstCol << setw(8) << secondCol << setw(0) << thirdCol << endl;
    cout << left << setw(12) << firstCol << setw(8) << secondCol << setw(0) << thirdCol << endl;
}

/******************************************************************************
*** FUNCTION: TACparser()                                                   ***
*******************************************************************************
*** DESCRIPTION : prints out converted TAC to new file                      ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::TACparser()
{
    string line = "";
    while (getline(infile, line))
    {
        if (line == "")
        {
            continue;
        }   
        
        if (getFirstWord(line) == "Proc")
        {
            emit(getSecondWord(line), "proc", ""); //cout << left << setw(12) << getSecondWord(line) << setw(0) << "proc" << endl;
            
            emit("", "push", "bp"); //cout << left << setw(12) << "" << setw(8) << "push" << setw(0) << "BP" << endl;

            emit("", "mov", "bp,sp"); //cout << left << setw(12) << "" << setw(8) << "mov" << setw(0) << "BP,SP" << endl;

            procVarPtr ptr = lookupProcVar(getSecondWord(line));
            if (ptr == NULL) // for main
            {
                emit("", "sub", "sp,0"); //cout << left << setw(12) << "" << setw(8) << "sub" << setw(0) << "BP,0" << endl;
                emit("", "", "");
            }
            else // for functions
            {
                emit("", "sub", "sp," + to_string(ptr->sizeLocals)); //cout << left << setw(12) << "" << setw(8) << "sub" << setw(0) << "BP," << ptr->sizeLocals<< endl;
                emit("", "", "");
            }
        } 
        else if (getFirstWord(line) == "Endp")
        {
             procVarPtr ptr = lookupProcVar(getSecondWord(line));
            if (ptr == NULL) // for main
            {
                emit("", "add", "sp,0"); //cout << left << setw(12) << "" << setw(8) << "sub" << setw(0) << "BP,0" << endl;
                emit("", "pop", "bp");
                emit("", "ret", "0");
            }
            else // for functions
            {
                emit("", "add", "sp," + to_string(ptr->sizeLocals)); //cout << left << setw(12) << "" << setw(8) << "sub" << setw(0) << "BP," << ptr->sizeLocals<< endl;
                emit("", "pop", "bp");
                emit("", "ret", to_string(ptr->sizeParams));
            }
            emit(getSecondWord(line), "endp", ""); //cout << left << setw(12) << getSecondWord(line) << setw(0) << "endp" << endl;
            emit("", "", "");
        } 
        else if ((getTotalWords(line) == 3) && (line.substr(0, 3) != "put") && (line.substr(0, 3) != "get") && (getSecondWord(line) == "="))
        {  
            CopyStat(getFirstWord(line), getThirdWord(line));
            emit("", "", "");
        }
        else if (((getTotalWords(line) == 5) && (line.substr(0, 3) != "put") && (line.substr(0, 3) != "get") && ((getFourthWord(line) == "+" ||(getFourthWord(line) == "-"))))) // for addition
        {
            AddStat(getThirdWord(line), getFifthWord(line), getFirstWord(line));
        }
        else if (((getTotalWords(line) == 5) && (line.substr(0, 3) != "put") && (line.substr(0, 3) != "get") && ((getFourthWord(line) == "*") || (getFourthWord(line) == "/"))))
        {
            MultiStat(getThirdWord(line), getFifthWord(line), getFirstWord(line));
        }
        else if (getFirstWord(line) == "push")
        {
            if (getSecondWord(line)[0] == '@')
            {
                string substr = getSecondWord(line).substr(1, line.size()-1);
                emit("", "push", "offset " + substr);
            }
            else
            {
                emit("", "push", getSecondWord(line));
            }
        }
        else if (getFirstWord(line) == "Call")
        {
            emit("", "call ", getSecondWord(line));
            emit("", "", "");
        }
        else if (getFirstWord(line) == "wrs") 
        {
            emit ("", "mov", "dx, offset " + getSecondWord(line));
            emit ("", "call", "writestr" );
            emit ("", "", "" );
        }
        else if (getFirstWord(line) == "wri") 
        {
            emit ("", "mov", "dx, " + formatOperand(getSecondWord(line)));
            emit ("", "call", "writeint");
            emit ("", "", "" );
        }
        else if (getFirstWord(line) == "wrln") 
        {
            emit ("", "call", "writeln");
            emit ("", "", "" );
        }
        else if (getFirstWord(line) == "rdi") 
        {
            emit ("", "call", "readint");
            emit("", "mov", formatOperand(getSecondWord(line)) + ", bx");
            emit ("", "", "" );
        }
        else if(getFirstWord(line)=="Start" && getSecondWord(line)=="proc")
        {
            emit("", "end start", "");
        }
    }
}

/******************************************************************************
*** FUNCTION: getFirstWord(string line)                                     ***
*******************************************************************************
*** DESCRIPTION : gets the first word of a line of strings                  ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : line.substr(0, line.find(" "))                                 ***
******************************************************************************/
string CodeGen::getFirstWord(string line)
{
    return line.substr(0, line.find(" "));
}

/******************************************************************************
*** FUNCTION: getSecondWord(string line)                                    ***
*******************************************************************************
*** DESCRIPTION : gets the second word of a line of strings                 ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : getFirstWord(line)                                             ***
******************************************************************************/
string CodeGen::getSecondWord(string line)
{
    line = line.substr(line.find(" ")+1, line.size()-line.find(" "));
    return getFirstWord(line);
}

/******************************************************************************
*** FUNCTION: getThirdWord(string line)                                     ***
*******************************************************************************
*** DESCRIPTION : gets the third word of a line of strings                  ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : getSecondWord(line)                                            ***
******************************************************************************/
string CodeGen::getThirdWord(string line)
{
    line = line.substr(line.find(" ")+1, line.size()-line.find(" "));
    return getSecondWord(line);
}

/******************************************************************************
*** FUNCTION: getFourthWord(string line)                                    ***
*******************************************************************************
*** DESCRIPTION : gets the fourth word of a line of strings                 ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : getThirdWord(line)                                             ***
******************************************************************************/
string CodeGen::getFourthWord(string line)
{
    line = line.substr(line.find(" ")+1, line.size()-line.find(" "));
    return getThirdWord(line);
}

/******************************************************************************
*** FUNCTION: getFifthWord(string line)                                     ***
*******************************************************************************
*** DESCRIPTION : gets the fifth word of a line of strings                  ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : getFourthWord(line)                                            ***
******************************************************************************/
string CodeGen::getFifthWord(string line)
{
    line = line.substr(line.find(" ")+1, line.size()-line.find(" "));
    return getFourthWord(line);
}

/******************************************************************************
*** FUNCTION: getTotalWords(string line)                                    ***
*******************************************************************************
*** DESCRIPTION : gets the total number of words in a line                  ***
*** INPUT ARGS : line                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : 1 or getTotalWords(sub)+1                                      ***
******************************************************************************/
int CodeGen::getTotalWords(string line) // Finds the total number of words in a line by counting # of spaces
{
    int position = line.find(" ");

    if(position != -1) // -1 if not found, otherwise found
    {
        string sub = line.substr(position + 1, line.size() - position - 1);
        return getTotalWords(sub)+1;
    }
    else 
    {
        return 1;
    }
}

/******************************************************************************
*** FUNCTION: CopyStat(string dest, string source)                          ***
*******************************************************************************
*** DESCRIPTION :  Converts an assignment statement into assembly code      ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::CopyStat(string dest, string source)
{
    //string code = "";
    //code.append(" mov ax,");
    //code.append(source);
    //emit(code);

    //code = "  mov ";
    //code.append(dest);
    //code.append(",ax");
    //emit(code);
    
    if (dest[0] == '@')
    {
        // erase @
        string substr = dest.substr(1, dest.size()-1);
        emit("", "mov", "bx," + formatOperand(substr)); // dest is now substr
        emit("", "mov", "ax," + formatOperand(source));
        emit("", "mov", "[bx],ax");
    }
    else
    {
        // source (value) to register
        emit("", "mov", "ax," + formatOperand(source)); //mov AX,5

        // register to destination (temporary variable)
        emit("", "mov", formatOperand(dest) + ",ax"); //mov 5,AX
    }

}

/******************************************************************************
*** FUNCTION: AddStat(string add1, string add2, string sum)                 ***
*******************************************************************************
*** DESCRIPTION :  Prints out the addopt statement assembly code            ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::AddStat(string add1, string add2, string sum)
{
    emit("", "mov", "ax," + formatOperand(add1));
    emit("", "add", "ax," + formatOperand(add2));
    emit("", "mov", formatOperand(sum) + ",ax");
    emit("", "", "");
}

/******************************************************************************
*** FUNCTION: MultiStat(string mult1, string mult2, string product)         ***
*******************************************************************************
*** DESCRIPTION :  Prints out the mulopt statement assembly code            ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void CodeGen::MultiStat(string mult1, string mult2, string product)
{
    emit("", "mov", "ax," + formatOperand(mult1));
    emit("", "mov", "bx," + formatOperand(mult2));
    emit("", "imul", "bx");
    emit("", "mov", formatOperand(product) + ",ax");
    emit("", "", "");
}

/******************************************************************************
*** FUNCTION: formatOperand(string str)                                     ***
*******************************************************************************
*** DESCRIPTION :  formats the operand before printing it out               ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
string CodeGen::formatOperand(string str) // if it's bp it will return " [ ] "
{
    if (isalnum(str[0]))
    {
        return str;
    }
    else if ((str[0] == '_') && (str[1] == 't'))
    {
        return str;
    }
    else if ((str[0] == '_') && (str[1] == 'b'))  
    {
        return "[" + str.substr(1, str.size()-1) + "]";
    }
    else
    {
        return "";
    }
}