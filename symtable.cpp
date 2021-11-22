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
#include <iomanip>
#include <string>
#include "symtable.h"

using namespace std;

int offset = 0;

SymbolTable::SymbolTable() // Symbol Table constructor - initiatlizes a symbol table to empty
{
    dataSegHead = NULL;
    procVarHead = NULL;
    for (int i = 0; i < TableSize; i++)
    {
        SymTab[i] = NULL;
    }
}

SymbolTable::~SymbolTable() // Symbol Table deconstructor
{
    EntryPtr ptr;
    EntryPtr tempptr;
    for (int i = 0; i < TableSize; i++)
    {
        ptr = SymTab[i];
        while (ptr != NULL) // Check if node is empty
        {
            if (ptr->TypeOfEntry == procedureEntry)
            {
                destroyParam(ptr->procedure.ParamList);
            }

            tempptr = ptr;
            ptr = ptr->next;
            delete tempptr;
        }
    }
}

/******************************************************************************
*** FUNCTION: hashpjw(string lex)                                           ***
*******************************************************************************
*** DESCRIPTION : Passes a lexeme and returns the location for that lexeme. ***
*** INPUT ARGS : lex                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
int SymbolTable::hashpjw(string lex)
{
    int h = 0;
    int g;
    for (int i = 0; i < lex.size(); i++)
    {
        h = (h << 24) + (lex[i]);
        g = h & 0xf0000000;
        if (g)
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % TableSize;
}

/******************************************************************************
*** FUNCTION: lookup(string lex)                                            ***
*******************************************************************************
*** DESCRIPTION : Uses lexeme to find entry and returns pointer to entry.   ***
*** INPUT ARGS : lex                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
EntryPtr SymbolTable::lookup(string lex) // Check depth if collision occurs
{
    int x;
    x = hashpjw(lex);

    EntryPtr ptr;
    ptr = SymTab[x];

    while (ptr != NULL)
    {
        if (ptr->Lexeme == lex && ptr->depth <= Depth)
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

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
void SymbolTable::insert(string lex, Symbol token, int depth)
{
    int x;
    x = hashpjw(lex);
    EntryPtr Entry;
    Entry = new TableEntry;
    if (Entry == NULL)
    {
        cerr << "Entry is FULL – No Insertion!" << endl;
    }
    else
    {
        Entry->Token = Token;
        Entry->Lexeme = lex;
        Entry->depth = depth;

        // Adding into the linked list from the front(head)
        Entry->next = SymTab[x];
        SymTab[x] = Entry;
    }
}

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
void SymbolTable::writeTable(int depth)
{
    EntryPtr ptr;
    for (int i = 0; i < TableSize; i++)
    {
        ptr = SymTab[i];
        while (ptr != NULL)
        {
            if (ptr->depth == depth)
            {
                cout << "Lexeme: " << ptr->Lexeme << endl;
                cout << "Class: " << ptr->TypeOfEntry << endl;
                // Debug code below to see type, size, and offset
                    // if (ptr->TypeOfEntry == varEntry)
                    // {
                    //     cout << "Type: " << ptr->var.TypeOfVariable << endl;
                    //     cout << "Size: " << ptr->var.size << endl;
                    //     cout << "Offset: " << ptr->var.Offset << endl;
                    // }
                    // else if (ptr->TypeOfEntry == constEntry)
                    // {
                    //     cout << "Type: " << ptr->constant.TypeOfConstant << endl;
                    //     if (ptr->constant.TypeOfConstant == intType)
                    //     {
                    //         cout << "Value: " << ptr->constant.Value << endl;
                    //     }
                    //     else
                    //     {
                    //         cout << "ValueR: " << ptr->constant.ValueR << endl;
                    //     }
                    // }
                    // else if (ptr->TypeOfEntry == procedureEntry)
                    // {
                    //         cout << "Size of Local Vars: " << ptr->procedure.SizeOfLocal << endl;
                    //         cout << "Size of Parameters: " << ptr->procedure.SizeOfParameters << endl;
                    //         viewParam(ptr->procedure.ParamList); 
                    // }
                    // else
                    //     cout << "There is no entry!" << endl;
                // Debug code above
            }
            ptr = ptr->next;
        }
    }
}

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
void SymbolTable::deleteDepth(int depth) //d = 2
{
    EntryPtr ptr;
    EntryPtr tempptr;

    for (int i = 0; i < TableSize; i++)
    {
        ptr = SymTab[i];
        if (ptr != NULL) // Check if node is empty
        {
            // Checks first node only (multiple first nodes if keep deleting)
            while (ptr->depth == depth)
            {
                tempptr = ptr;          // Store pointer address
                SymTab[i] = ptr->next;  // Set SymTab[i] equal to next pointer
                ptr = SymTab[i];        // Traverse to next entry
                delete tempptr;         // Delete entry
                if (ptr == NULL)
                    break;
            }

            // Checks all other nodes besides first node
            if (ptr != NULL)
            {
                while (ptr->next != NULL)
                {
                    if ((ptr->next)->depth == depth)
                    {
                        tempptr = ptr->next;
                        ptr->next = (ptr->next)->next;
                        delete(tempptr);
                    }
                    else
                    {
                        ptr = ptr->next; // When (ptr->next)->depth != depth
                    }
                }
            }
        }
    }
}

/******************************************************************************
*** FUNCTION: insertParam(ParamPtr &Param, VarType type, ParamMode mode)    ***
*******************************************************************************
*** DESCRIPTION : Inserts the parameters of different types of entries      ***
*** INPUT ARGS : ParamPtr &Param, VarType type, ParamMode mode              ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::insertParam(ParamPtr& Param, VarType type, ParamMode mode)
{
    ParamPtr temp = Param;
    Param = new ParamNode;
    if (Param == NULL)
    {
        cerr << "Parameter is FULL – No Insertion!" << endl;
    }

    else
    {
        Param->typeOfParameter = type;
        Param->typeOfMode = mode;
        Param->Next = temp;
    }
}

/******************************************************************************
*** FUNCTION: destroyParam(ParamPtr &Param)                                 ***
*******************************************************************************
*** DESCRIPTION : Delete the parameters in the symbol table to clear space  ***
*** INPUT ARGS : ParamPtr &Param                                            ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::destroyParam(ParamPtr& Param) // post order traversal
{
    if (Param != NULL)
    {
        destroyParam(Param->Next);
        delete Param; // Visit Current Node
        Param = NULL;
    }
}

/******************************************************************************
*** FUNCTION: viewParam(ParamPtr &Param)                                    ***
*******************************************************************************
*** DESCRIPTION : Prints out the linked list of paramters and info          ***
*** INPUT ARGS : ParamPtr &Param                                            ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::viewParam(ParamPtr& Param)
{
    if (Param != NULL)
    {
        cout << "Mode: " << Param->typeOfMode << endl;
        cout << "Parameter Type: " << Param->typeOfParameter << endl;
        viewParam(Param->Next);
    }
}


/******************************************************************************
*** FUNCTION: insertDataSeg(dataSegPtr& dataSeg, string variable, string value)
*******************************************************************************
*** DESCRIPTION : inserts one variable of depth 1 into linked list          ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::insertDataSeg(dataSegPtr& dataSeg, string variable, string value)
{
    if (dataSeg == NULL)
    {
        dataSeg = new dataSegNode;
        if (dataSeg == NULL)
        {
            cerr << "Node FULL – No Insertion" << endl;
        }
        else
        {
            dataSeg->variable = variable;
            dataSeg->value = value;
            dataSeg->next = NULL;
        }
    }
    else
    {
        insertDataSeg(dataSeg->next, variable, value);
    }
    return;
}

/******************************************************************************
*** FUNCTION: destroyDataSeg(dataSegPtr& dataSeg)                           ***
*******************************************************************************
*** DESCRIPTION : deletes data seg linked list                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::destroyDataSeg(dataSegPtr& dataSeg)
{
    if (dataSeg != NULL)
    {
        destroyDataSeg(dataSeg->next);
        delete dataSeg; // Visit Current Node
        dataSeg = NULL;
    }
    return;
}

/******************************************************************************
*** FUNCTION: writeDataSeg()                                                ***
*******************************************************************************
*** DESCRIPTION : inserts all variables data seg into linked list           ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::writeDataSeg()
{
    EntryPtr ptr;
    for (int i = 0; i < TableSize; i++)
    {
        ptr = SymTab[i];
        while (ptr != NULL)
        {
            if (ptr->depth == 1 && ptr->TypeOfEntry == varEntry)
            {
                insertDataSeg(dataSegHead, ptr->Lexeme, "");
                //cout << ptr->Lexeme << endl;
            }

            if (ptr->depth == 1 && ptr->TypeOfEntry == literalEntry)
            {
                int i = 0;
                string str = "";
                while (ptr->literals.str[i] != '\0')
                {
                    str.push_back(ptr->literals.str[i++]);
                }
                insertDataSeg(dataSegHead, ptr->Lexeme, str);
                //cout << ptr->Lexeme << endl;
            }
            ptr = ptr->next;
        }
    }
    //dataSegHead = segPtr;
}

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
void SymbolTable::insertProcVar(procVarPtr& procVar, string variable, int sizeLocals, int sizeParams, int depth)
{
    if (procVar == NULL)
    {
        procVar = new procVarNode;
        if (procVar == NULL)
        {
            cerr << "Node FULL – No Insertion" << endl;
        }
        else
        {
            procVar->variable = variable;
            procVar->sizeLocals = sizeLocals;
            procVar->sizeParams = sizeParams;
            procVar->depth = depth;
            procVar->next = NULL;
        }
    }
    else
    {
        insertProcVar(procVar->next, variable, sizeLocals, sizeParams, depth);
    }
    return;
}

/******************************************************************************
*** FUNCTION: destroyProcVar(procVarPtr& procVar                            ***
*******************************************************************************
*** DESCRIPTION : destroys procVar linked list                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::destroyProcVar(procVarPtr& procVar)
{
    if (procVar != NULL)
    {
        destroyProcVar(procVar->next);
        delete procVar; // Visit Current Node
        procVar = NULL;
    }
    return;
}

/******************************************************************************
*** FUNCTION: writeProcVar(int depth)                                       ***
*******************************************************************************
*** DESCRIPTION : writes all proc func names into linked list               ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::writeProcVar(int depth)
{
    EntryPtr ptr;
    for (int i = 0; i < TableSize; i++)
    {
        ptr = SymTab[i];
        while (ptr != NULL)
        {
            if (ptr->depth == depth && ptr->TypeOfEntry == procedureEntry)
            {
                insertProcVar(procVarHead, ptr->Lexeme, ptr->procedure.SizeOfLocal, ptr->procedure.SizeOfParameters, ptr->depth);
            }
            ptr = ptr->next;
        }
    }
    //procVarHead = procPtr;
}


/******************************************************************************
*** FUNCTION: lookupProcVar(string variable)                                ***
*******************************************************************************
*** DESCRIPTION : lookup names for all procedure names                      ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
procVarPtr SymbolTable::lookupProcVar(string variable)
{
    procVarPtr ptr = procVarHead;
    while (ptr != NULL)
    {
        if (ptr->variable == variable)
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

/******************************************************************************
*** FUNCTION: viewProcVar()                                                 ***
*******************************************************************************
*** DESCRIPTION : displays procvar linked list                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void SymbolTable::viewProcVar()
{
    procVarPtr ptr = procVarHead;
    while (ptr != NULL)
    {
        cout << ptr->variable << endl;
        ptr = ptr->next;
    }
}

/******************************************************************************
*** FUNCTION: mainProcVar()                                                 ***
*******************************************************************************
*** DESCRIPTION : returns pointer of main func                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
procVarPtr SymbolTable::mainProcVar()
{
    procVarPtr ptr = procVarHead;
    while (ptr != NULL)
    {
        if (ptr->depth == 0)
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}
