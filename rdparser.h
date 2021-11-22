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
#ifndef _RDPARSER_H
#define _RDPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include "lex.h"
#include "symtable.h"

using namespace std;

class Parser : public Lexical, public SymbolTable
{
public:

    using Lexical::Lexical; // Inherited Constructor
    using SymbolTable::SymbolTable;

    /******************************************************************************
    *** FUNCTION: Match(Symbol desired)                                         ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the grammar         ***
    *** INPUT ARGS : desired                                                    ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Match(Symbol desired);

    /******************************************************************************
    *** FUNCTION: Prog()                                                        ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Prog grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Prog();

    /******************************************************************************
    *** FUNCTION: Type()                                                        ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Type grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Type(VarType& TypVal, int& varTotal, int& varCount);

    /******************************************************************************
    *** FUNCTION: ValueFunc()                                                   ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Value grammar   ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ValueFunc(VarType& TypVal);

    /******************************************************************************
    *** FUNCTION: Procedures()                                                  ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Proc grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Procedures();

    /******************************************************************************
    *** FUNCTION: Args()                                                        ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Args grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Args(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: ArgList()                                                     ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the AList grammar   ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ArgList(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Mode()                                                        ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Mode grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Mode(ParamMode& mode);

    /******************************************************************************
    *** FUNCTION: SeqOfStmts()                                                  ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the SoS grammar     ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void SeqOfStmts(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: StatTail()                                                    ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the StatTail gramr  ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void StatTail(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Statement()                                                   ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Statement gramr ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Statement(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: AssignStat()                                                  ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the AssignStat gramr***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void AssignStat(EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Expr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)           ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Expr grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Expr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Relation(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)       ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Relation grammar***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Relation(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: SimpleExpr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)     ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the SimpleExpr gramr***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void SimpleExpr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: MoreTerm(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)       ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the MoreTerm grammar***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void MoreTerm(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Term(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)           ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Term grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Term(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: MoreFactor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)     ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the MoreFactor gramr***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void MoreFactor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Factor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)         ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Factor grammar  ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Factor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: Vars(EntryPtr& ProcPtr, int& varTotal, int& varCount)         ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Vars grammar    ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : ProcPtr, varTotal, varCount                               ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Vars(EntryPtr& ProcPtr, int& varTotal, int& varCount);

    /******************************************************************************
    *** FUNCTION: ArgTail(EntryPtr& ProcPtr, VarType &TypVal,                   ***
    ***           EntryType &EntryVal, ParamMode mode)                          ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the ArgTail grammar ***
    *** INPUT ARGS : ProcPtr, mode                                              ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : TypVal, EntryVal                                          ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ArgTail(EntryPtr& ProcPtr, VarType& TypVal, EntryType& EntryVal, ParamMode mode);

    /******************************************************************************
    *** FUNCTION: VarTail(EntryPtr& ProcPtr, VarType& TypVal, EntryType&        ***
    ***            EntryVal, int& varTotal, int& varCount)                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the VarTail grammar ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : ProcPtr, TypVal, EntryVal, varTotal, varCount             ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void VarTail(EntryPtr& ProcPtr, VarType& TypVal, EntryType& EntryVal, int& varTotal, int& varCount);

    /******************************************************************************
    *** FUNCTION: VarTail2(EntryPtr& ProcPtr, VarType& TypVal, EntryType&       ***
    ***            EntryVal, int& varTotal, int& varCount)                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the VarTail2 grammar***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : ProcPtr, TypVal, EntryVal, varTotal, varCount             ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void VarTail2(VarType& TypVal, EntryType& EntryVal, int& varTotal, int& varCount);

    /******************************************************************************
    *** FUNCTION: checkForDups()                                                ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if there are duplicate tokens               ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void checkForDups();

    /******************************************************************************
    *** FUNCTION: checkForUndecl()                                              ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if there are undeclared tokens              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void checkForUndecl();

    /******************************************************************************
    *** FUNCTION: newTemp()                                                     ***
    *******************************************************************************
    *** DESCRIPTION : creates a temporary variable                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    EntryPtr newTemp();

    /******************************************************************************
    *** FUNCTION: emit(string code)                                             ***
    *******************************************************************************
    *** DESCRIPTION : prints the TAC into a new file                            ***
    *** INPUT ARGS : code                                                       ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void emit(string code);

    /******************************************************************************
    *** FUNCTION: newBP(string str, EntryPtr ptr, EntryPtr& ProcPtr)            ***
    *******************************************************************************
    *** DESCRIPTION : makes a new temporary variable for depth >= 2             ***
    *** INPUT ARGS : str, ptr                                                   ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : ProcPtr                                                   ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    EntryPtr newBP(string str, EntryPtr ptr, EntryPtr& ProcPtr);

    /******************************************************************************
    *** FUNCTION: typeAppend(string& code, EntryPtr ptr)                        ***
    *******************************************************************************
    *** DESCRIPTION : appends the correct TAC code for the given ptr            ***
    *** INPUT ARGS : ptr                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : code                                                      ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void typeAppend(string& code, EntryPtr ptr);

    /******************************************************************************
    *** FUNCTION: ProcCall(EntryPtr Ptr)                                        ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the ProcCall grammar***
    *** INPUT ARGS : ptr                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ProcCall(EntryPtr Ptr);

    /******************************************************************************
    *** FUNCTION: Params(ParamPtr ptr)                                          ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Params grammar  ***
    *** INPUT ARGS : ptr                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Params(ParamPtr ParamList);

    /******************************************************************************
    *** FUNCTION: ParamsTail(ParamPtr ptr)                                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the ParamsTaill grmr***
    *** INPUT ARGS : ptr                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void ParamsTail(ParamPtr ParamList);

    /******************************************************************************
    *** FUNCTION: IOStat()                                                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the IOStat grammar  ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void IO_Stat();

    /******************************************************************************
    *** FUNCTION:In_Stat()                                                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the In_Stat grammar ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void In_Stat();

    /******************************************************************************
    *** FUNCTION:Id_List()                                                      ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Id_List grammar ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Id_List();

    /******************************************************************************
    *** FUNCTION:Id_List_Tail()                                                 ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Id_List_tail grm***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Id_List_Tail();

    /******************************************************************************
    *** FUNCTION:Out_Stat()                                                     ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Out_Stat grammar***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Out_Stat();

    /******************************************************************************
    *** FUNCTION: Write_List(Symbol opcode)                                     ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Write_List gramr***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Write_List(Symbol opcode);

    /******************************************************************************
    *** FUNCTION: Write_List_Tail(Symbol opcode)                                ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Write_List_Tail gm*
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Write_List_Tail(Symbol opcode);

    /******************************************************************************
    *** FUNCTION: Write_Token(EntryPtr& ptr)                                    ***
    *******************************************************************************
    *** DESCRIPTION : checks to see if actual Token matches the Write_Token grmr***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    void Write_Token(EntryPtr& ptr);

    /******************************************************************************
    *** FUNCTION: newLiteral(string str)                                        ***
    *******************************************************************************
    *** DESCRIPTION : creates a temporary variable                              ***
    *** INPUT ARGS : N/A                                                        ***
    *** OUTPUT ARGS : N/A                                                       ***
    *** IN/OUT ARGS : N/A                                                       ***
    *** RETURN : N/A                                                            ***
    ******************************************************************************/
    EntryPtr newLiteral(string str);
};
#endif