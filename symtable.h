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
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <iostream>
#include <string>
#include "lex.h"

using namespace std;

extern int offset;

const int TableSize = 211; // Use a prime number

enum VarType { charType, intType, floatType }; // Creates an enumerated type VarType to store character type, integer type, and float type variables

enum EntryType { constEntry, varEntry, procedureEntry, literalEntry }; // Creates an enumerated type EntryType to store constant type, variable type, and procedure type

enum ParamMode { inMode, outMode, inoutMode }; // Creates an enumerated type ParamMode to store inMode type, outMode type, and inoutMode type

struct ParamNode;  // Linked list of arguments in procedure
typedef ParamNode* ParamPtr; // ParamNode *ptr; is the same as: paramPtr ptr;

struct ParamNode
{
    VarType typeOfParameter;
    ParamMode typeOfMode;
    ParamPtr Next;
};

struct TableEntry;  // Linked list of hash table
typedef TableEntry* EntryPtr; // Pointer to actual table entry

struct TableEntry
{
    Symbol Token;
    string Lexeme;
    int depth;
    EntryType TypeOfEntry; // Tag field for the union
    union
    {
        struct
        {
            VarType TypeOfVariable;
            int Offset;
            int size;
            ParamMode typeOfMode;
        } var;

        struct
        {
            VarType TypeOfConstant; // int or float constant
            union
            {
                int Value;
                float ValueR;
            };
        } constant;

        struct
        { // this is the entry for a function
            int LastOffset; // Last Offset 
            int SizeOfLocal; // Size of local variables inside the function
            int SizeOfParameters; // Size of variables inside the parameter
            ParamPtr ParamList; // Linked list of paramter types
        } procedure;

        struct
        {
            char str[50];
        } literals;


    }; // End of union
    EntryPtr next; // Points to next list item
};

struct dataSegNode;
typedef dataSegNode* dataSegPtr;

struct dataSegNode
{
    string variable;
    string value;
    dataSegPtr next;
};

struct procVarNode;
typedef procVarNode* procVarPtr;

struct procVarNode
{
    string variable;
    int sizeLocals;
    int sizeParams;
    int depth;
    procVarPtr next;
};

class SymbolTable
{
private:
    /******************************************************************************
    *** FUNCTION: hashpjw(string lex)                                           ***
    *******************************************************************************
    *** DESCRIPTION : Passes a lexeme and returns the location for that lexeme. ***
    *** INPUT ARGS : lex                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    int hashpjw(string lex);

public:

    dataSegPtr dataSegHead;
    procVarPtr procVarHead;
    EntryPtr SymTab[TableSize]; // Symbol Table

    SymbolTable(); // Constructor
    ~SymbolTable(); // Destructor


    // For table entries linked list

    /******************************************************************************
    *** FUNCTION: lookup(string lex)                                            ***
    *******************************************************************************
    *** DESCRIPTION : Uses lexeme to find entry and returns pointer to entry.   ***
    *** INPUT ARGS : lex                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    EntryPtr lookup(string lex);

    /******************************************************************************
    *** FUNCTION: insert(string lex, Symbol token, int depth)                   ***
    *******************************************************************************
    *** DESCRIPTION : Inserts lexeme, token, and depth into a record in the     ***
    *** Symbol Table.                                                           ***
    *** INPUT ARGS : lex, token, depth                                          ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void insert(string lex, Symbol token, int depth);

    /******************************************************************************
    *** FUNCTION: writeTable(int depth)                                         ***
    *******************************************************************************
    *** DESCRIPTION : Writes out all variables (lexeme only) that are in the    ***
    *** table at a specified depth.                                             ***
    *** INPUT ARGS : depth                                                      ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void writeTable(int depth);

    /******************************************************************************
    *** FUNCTION: deleteDepth(int depth)                                        ***
    *******************************************************************************
    *** DESCRIPTION : Delete is passed the depth and deletes all records tha    ***
    *** are in the table at that depth.                                         ***
    *** INPUT ARGS : depth                                                      ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void deleteDepth(int depth);


    // For parameters linked list

    /******************************************************************************
    *** FUNCTION: insertParam(ParamPtr &Param, VarType type, ParamMode mode)    ***
    *******************************************************************************
    *** DESCRIPTION : Inserts the parameters of different types of entries      ***
    *** INPUT ARGS : ParamPtr &Param, VarType type, ParamMode mode              ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void insertParam(ParamPtr& Param, VarType type, ParamMode mode);

    /******************************************************************************
    *** FUNCTION: destroyParam(ParamPtr &Param)                                 ***
    *******************************************************************************
    *** DESCRIPTION : Delete the parameters in the symbol table to clear space  ***
    *** INPUT ARGS : ParamPtr &Param                                            ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void destroyParam(ParamPtr& Param); // post order traversal

    /******************************************************************************
    *** FUNCTION: viewParam(ParamPtr &Param)                                    ***
    *******************************************************************************
    *** DESCRIPTION : Prints out the linked list of paramters and info          ***
    *** INPUT ARGS : ParamPtr &Param                                            ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void viewParam(ParamPtr& Param);

    /******************************************************************************
    *** FUNCTION: insertDataSeg(dataSegPtr& dataSeg, string variable, string value)
    *******************************************************************************
    *** DESCRIPTION : inserts one variable of depth 1 into linked list          ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void insertDataSeg(dataSegPtr& dataSeg, string variable, string value);

    /******************************************************************************
    *** FUNCTION: destroyDataSeg(dataSegPtr& dataSeg)                           ***
    *******************************************************************************
    *** DESCRIPTION : deletes data seg linked list                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void destroyDataSeg(dataSegPtr& dataSeg);

    /******************************************************************************
    *** FUNCTION: writeDataSeg()                                                ***
    *******************************************************************************
    *** DESCRIPTION : inserts all variables data seg into linked list           ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void writeDataSeg();

    /******************************************************************************
    *** FUNCTION: insertProcVar(procVarPtr& procVar, string variable,           ***
    ***            int sizeLocals, int sizeParams, int depth)                   ***
    *******************************************************************************
    *** DESCRIPTION : inserts a proc func name into linked list                 ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void insertProcVar(procVarPtr& procVar, string variable, int sizeLocals, int sizeParams, int depth);

    /******************************************************************************
    *** FUNCTION: destroyProcVar(procVarPtr& procVar                            ***
    *******************************************************************************
    *** DESCRIPTION : destroys procVar linked list                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void destroyProcVar(procVarPtr& procVar);

    /******************************************************************************
    *** FUNCTION: writeProcVar(int depth)                                       ***
    *******************************************************************************
    *** DESCRIPTION : writes all proc func names into linked list               ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void writeProcVar(int depth);

    /******************************************************************************
    *** FUNCTION: lookupProcVar(string variable)                                ***
    *******************************************************************************
    *** DESCRIPTION : lookup names for all procedure names                      ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    procVarPtr lookupProcVar(string variable);

    /******************************************************************************
    *** FUNCTION: viewProcVar()                                                 ***
    *******************************************************************************
    *** DESCRIPTION : displays procvar linked list                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void viewProcVar();

    /******************************************************************************
    *** FUNCTION: mainProcVar()                                                 ***
    *******************************************************************************
    *** DESCRIPTION : returns pointer of main func                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    procVarPtr mainProcVar();

};

#endif