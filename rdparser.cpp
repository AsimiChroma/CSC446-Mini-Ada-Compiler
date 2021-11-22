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
#include <string>
#include <fstream>

using namespace std;

/******************************************************************************
*** FUNCTION: Match(Symbol desired)                                         ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the grammar         ***
*** INPUT ARGS : desired                                                    ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Match(Symbol desired)
{
    //cout << "Token: " << symbols[Token] << endl;
    if (Token == desired)
    {
        GetNextToken();
    }
    else if (signopt == desired && Token == addopt && Lexeme != "OR") // Use addopt to check for signopt - if the Token is +, -, and not "or", then instead of being an addpot it is a signopt
    {
        GetNextToken();
    }
    else
    {
        cout << "Error on line " << LineNo << ": expecting " << symbols[desired] << " but found " << symbols[Token] << endl;
        exit(1);
    }
}

/******************************************************************************
*** FUNCTION: Prog()                                                        ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Prog grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Prog() /* Implements Prog -> procdeure idt Args is
                                           Vars
                                           Procedures
                                           begin
                                           SeqOfStatements
                                           end idt; */
{
    //cout << "Prog " << endl;
    Match(proceduret);
    int varCount = 0;
    int varTotal = 0;
    string code = "";
    string ProcId = Lexeme;
    //cout << "ProcId: " << ProcId << endl;
    EntryPtr ProcPtr;
    checkForDups();
    insert(Lexeme, Token, Depth);
    ProcPtr = lookup(Lexeme);
    ProcPtr->procedure.ParamList = NULL;
    ProcPtr->TypeOfEntry = procedureEntry;
    ProcPtr->procedure.LastOffset = 0;

    Match(idt);
    Depth++; // After first idt is matched, increase the depth by 1
    offset = 4; // Prepare Offset as 4 for args

    Args(ProcPtr); // for parameter variables
    Match(ist);

    offset = -2; //Prepare Offset as -2 for vars
    int initial = offset;
    Vars(ProcPtr, varTotal, varCount); // Changed from DeclarativePart into Vars; for local variables
    ProcPtr->procedure.SizeOfLocal = offset - initial; // Keeps track of total size of variables in the body of the function
    Procedures();
    Match(begint);

    code.append("Proc ");
    code.append(ProcPtr->Lexeme);
    emit(code);
    code = "";

    SeqOfStmts(ProcPtr);
    Match(endt);

    code.append("Endp ");
    code.append(ProcPtr->Lexeme);
    emit(code);
    code = "";
    emit(code); //Emit 2nd time for new line

    string EndId = Lexeme;
    //cout << "EndId: " << EndId << endl;
    Match(idt);

    if (EndId != ProcId)
    {
        cout << "Error on line " << LineNo << ": procedure idt doesn't match end idt" << endl;
        exit(1);
    }

    Match(semit);
    //cout << "------------------------" << endl;
    //cout << "Current Depth: " << Depth << endl;
    //writeTable(Depth);
    if (Depth == 1)
    {
        writeDataSeg(); // Writes data segments (variables in depth 1) into a linked list
    }
    writeProcVar(Depth); // Makes linked list of procecure names
    deleteDepth(Depth--);

    if (Depth == 0)
    {
        writeProcVar(Depth);
        code.append("Start proc ");
        code.append(ProcPtr->Lexeme);
        emit(code);
    }
}

/******************************************************************************
*** FUNCTION: Type()                                                        ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Type grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Type(VarType& TypVal, int& varTotal, int& varCount) // Implements Type -> integert | floatt | chart  // For EntryType (variable, constant, or procedure)
{
    //cout << "Type " << endl;
    if (Token == integert) // If token is an INTEGER token then set the type of value inserted to integer 
    {
        varTotal = varTotal + varCount * 2;
        varCount = 0;
        TypVal = intType;
        Match(integert);
    }
    else if (Token == floatt) // If token is a FLOAT token then set the type of value inserted to float 
    {
        varTotal = varTotal + varCount * 4;
        varCount = 0;
        TypVal = floatType;
        Match(floatt);
    }
    else if (Token == chart) // If token is a CHAR token then set the type of value inserted to char
    {
        varTotal = varTotal + varCount * 1;
        varCount = 0;
        TypVal = charType;
        Match(chart);
    }
    else if (Token == constantt) // Else if token is a CONSTANT token then set the type of value inserted to constant
    {
        Match(constantt);
        Match(assignopt);
        ValueFunc(TypVal);
    }
}

/******************************************************************************
*** FUNCTION: ValueFunc()                                                   ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Value grammar   ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::ValueFunc(VarType& TypVal) // Implements ValueFunc -> NumericalLiteral
{
    //cout << "ValueFunc " << endl;
    if (Lexeme.find('.') == -1) // If there isn't a . then set value to be integer value
    {
        TypVal = intType;
    }
    else // Else if there is a . then set valueR to be float value
    {
        TypVal = floatType;
    }
    Match(numt);
}

/******************************************************************************
*** FUNCTION: Procedures()                                                  ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Proc grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Procedures() // Implements Procedures -> Prog Procedures | e
{
    //cout << "Procedures " << endl;
    if (Token == proceduret)
    {
        Prog();
        Procedures();
    }
}

/******************************************************************************
*** FUNCTION: Args()                                                        ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Args grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Args(EntryPtr& ProcPtr) // Implements Args -> ( ArgList ) | e
{
    //cout << "Args " << endl;
    int initial = offset;
    if (Token == lparent)
    {
        Match(lparent);
        ArgList(ProcPtr);
        Match(rparent);
    }
    ProcPtr->procedure.SizeOfParameters = offset - initial;
}

/******************************************************************************
*** FUNCTION: ArgList()                                                     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the AList grammar   ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::ArgList(EntryPtr& ProcPtr) // Implements ArgList -> Mode idt ArgTail
{
    //cout << "ArgList " << endl;
    VarType TypVal;
    EntryType EntryVal;
    ParamMode mode;
    EntryPtr ptr;

    // First find the mode that the variable is in and then insert all its info at that level into table
    Mode(mode);
    checkForDups();
    insert(Lexeme, Token, Depth);
    ptr = lookup(Lexeme);

    Match(idt);
    ptr->var.typeOfMode = mode;

    ArgTail(ProcPtr, TypVal, EntryVal, mode);
    insertParam(ProcPtr->procedure.ParamList, TypVal, mode);

    ptr->TypeOfEntry = EntryVal;

    // Put in size and offset into linkedlist
    ptr->var.TypeOfVariable = TypVal;
    ptr->var.Offset = offset;

    if (TypVal == charType)
    {
        ptr->var.size = 1;
        offset = offset + 1;
    }
    else if (TypVal == intType)
    {
        ptr->var.size = 2;
        offset = offset + 2;
    }
    else if (TypVal == floatType)
    {
        ptr->var.size = 4;
        offset = offset + 4;
    }
}

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
void Parser::ArgTail(EntryPtr& ProcPtr, VarType& TypVal, EntryType& EntryVal, ParamMode mode) // Implements ArgTail -> , idt ArgTail | : Type ; ArgList | : Type
{
    //cout << "ArgTail" << endl;
    if (Token == commat)
    {
        EntryPtr ptr;

        Match(commat);
        checkForDups();
        insert(Lexeme, Token, Depth);
        ptr = lookup(Lexeme);

        Match(idt);
        ptr->var.typeOfMode = mode;

        ArgTail(ProcPtr, TypVal, EntryVal, mode);
        insertParam(ProcPtr->procedure.ParamList, TypVal, mode);

        ptr->TypeOfEntry = EntryVal;

        // Put in size and offset into linkedlist
        ptr->var.TypeOfVariable = TypVal;
        ptr->var.Offset = offset;
        ptr->var.typeOfMode = mode;
        //insertParam(ProcPtr->procedure.ParamList, TypVal, mode);

        if (TypVal == charType)
        {
            ptr->var.size = 1;
            offset = offset + 1;
        }
        else if (TypVal == intType)
        {
            ptr->var.size = 2;
            offset = offset + 2;
        }
        else if (TypVal == floatType)
        {
            ptr->var.size = 4;
            offset = offset + 4;
        }
    }
    else if (Token == colont)
    {
        Match(colont);
        EntryVal = varEntry;
        int empty = 0;
        Type(TypVal, empty, empty);

        if (Token == semit)
        {
            Match(semit);
            ArgList(ProcPtr);
        }
    }

    else
    {
        cout << "Error on line " << LineNo << ": expecting " << ", " << symbols[commat] << ", or " << symbols[colont] << " but found " << symbols[Token] << endl;
        exit(1);
    }
}

/******************************************************************************
*** FUNCTION: Mode()                                                        ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Mode grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Mode(ParamMode& mode) // Implements Mode -> in | out | inout | e
{
    //cout << "Mode " << endl;
    if (Token == intok)
    {
        mode = inMode;
        Match(intok);
    }
    else if (Token == outt)
    {
        mode = outMode;
        Match(outt);
    }
    else if (Token == inoutt)
    {
        mode = inoutMode;
        Match(inoutt);
    }
    else // Default option of in mode if no mode is listed
    {
        mode = inMode;
    }
}

/******************************************************************************
*** FUNCTION: SeqOfStmts()                                                  ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the SoS grammar     ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::SeqOfStmts(EntryPtr& ProcPtr) // Implements SeqOfStatements -> Statement ; StatTail | e
{
    //cout << "SOS " << endl;
    if (Token != endt)
    {
        Statement(ProcPtr);
        Match(semit);
        StatTail(ProcPtr);
    }
}

/******************************************************************************
*** FUNCTION: StatTail()                                                    ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the StatTail gramr  ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::StatTail(EntryPtr& ProcPtr) // Implements StatTail -> Statement ; StatTail | e
{
    if (Token != endt)
    {
        Statement(ProcPtr);
        Match(semit);
        StatTail(ProcPtr);
    }
}

/******************************************************************************
*** FUNCTION: Statement()                                                   ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Statement gramr ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Statement(EntryPtr& ProcPtr) // Implements Statement -> AssignStat | IOStat
{
    if (Token == idt)
    {
        AssignStat(ProcPtr);
    }
    else
    {
        IO_Stat();
    }
}

/******************************************************************************
*** FUNCTION: AssignStat()                                                  ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the AssignStat gramr***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::AssignStat(EntryPtr& ProcPtr) // Implements AssignStat -> idt := Expr | ProcCall
{
    if (lookup(Lexeme)->TypeOfEntry != procedureEntry)
    {
        if (Depth == 1) //------------Depth is equal to 1------------
        {
            EntryPtr idtptr = NULL;
            EntryPtr ptr = NULL; // for synthesize
            string str = "";
            string code = "";
            idtptr = lookup(Lexeme);

            // Check for undeclared idt before matching idt
            checkForUndecl();
            Match(idt);
            Match(assignopt);
            Expr(str, ptr, ProcPtr);

            // Checks for odd scenario where ptr return from Expr() is not temp; if there is no +, -, /, or *
            if (ptr == NULL) // if it's a number
            {
                EntryPtr tempptr = newTemp();
                code.append(tempptr->Lexeme);
                code.append(" = ");
                code.append(str);
                emit(code);
                code = "";
                ptr = tempptr;
            }

            if (ptr->Lexeme.substr(0, 2) != "_t") // if it's a variable
            {
                EntryPtr tempptr = newTemp();
                code.append(tempptr->Lexeme);
                code.append(" = ");
                typeAppend(code, ptr);
                emit(code);
                code = "";
                ptr = tempptr;
            }

            code.append(idtptr->Lexeme);
            code.append(" = ");
            code.append(ptr->Lexeme);
            emit(code); // Emit prints out to TAC file
        }
        else //------------Depth is greater than or equal to 2------------
        {
            EntryPtr idtptr = NULL;
            EntryPtr ptr = NULL; // for synthesize
            string str = "";
            string code = "";
            idtptr = lookup(Lexeme);

            // Check for undeclared idt before matching idt
            checkForUndecl();
            Match(idt);
            Match(assignopt);
            Expr(str, ptr, ProcPtr);

            if (ptr == NULL)
            {
                EntryPtr bpptr = newBP(str, ptr, ProcPtr);
                typeAppend(code, bpptr);
                code.append(" = ");
                code.append(str);
                emit(code);
                code = "";
                ptr = bpptr;
            }

            //checks for odd scenario where ptr return from Expr() is not temp
            if (ptr->Lexeme.substr(0, 3) != "_bp")
            {
                EntryPtr bpptr = newBP(str, ptr, ProcPtr);
                typeAppend(code, bpptr);
                code.append(" = ");
                typeAppend(code, ptr);
                emit(code);
                code = "";
                ptr = bpptr;
            }
            typeAppend(code, idtptr);
            code.append(" = ");
            typeAppend(code, ptr);
            emit(code); // emit prints out to TAC file
        }
    }
    else //Proc call
    {
        ProcCall(ProcPtr);
    }
}

/******************************************************************************
*** FUNCTION: Expr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)           ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Expr grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Expr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements Expr -> Relation
{
    Relation(str, ptr, ProcPtr);
}

/******************************************************************************
*** FUNCTION: Relation(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)       ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Relation grammar***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Relation(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements Relation -> SimpleExpr
{
    SimpleExpr(str, ptr, ProcPtr);
}

/******************************************************************************
*** FUNCTION: SimpleExpr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the SimpleExpr gramr***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::SimpleExpr(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements SimpleExpr -> Term MoreTerm
{
    Term(str, ptr, ProcPtr);
    MoreTerm(str, ptr, ProcPtr);
}

/******************************************************************************
*** FUNCTION: MoreTerm(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)       ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the MoreTerm grammar***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::MoreTerm(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements MoreTerm -> addopt Term MoreTerm | e
{
    if (Token == addopt)
    {
        if (Depth == 1) //------------Depth is equal to 1------------
        {
            string inherit_str = "";
            EntryPtr inherit_ptr = NULL;
            string code = "";
            string operation = Lexeme; // Save if + or -

            Match(addopt);
            Term(inherit_str, inherit_ptr, ProcPtr);

            // Code from lecture
            EntryPtr tempptr = newTemp();
            code.append(tempptr->Lexeme);
            code.append(" = ");

            // Check if a number or a variable for higher level (grammar level)
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            code.append(" ");
            code.append(operation);
            code.append(" ");

            // Check if a number or a variable for lower level (grammar level)
            if (inherit_ptr == NULL)
            {
                code.append(inherit_str);
            }
            else
            {
                typeAppend(code, inherit_ptr);
            }

            emit(code);

            str = "";
            ptr = tempptr;
            MoreTerm(str, ptr, ProcPtr);
        }
        else //------------Depth is greater than or equal to 2------------
        {
            string inherit_str = "";
            EntryPtr inherit_ptr = NULL;
            string code = "";
            string operation = Lexeme;

            Match(addopt);
            Term(inherit_str, inherit_ptr, ProcPtr);

            EntryPtr bpptr = newBP(str, ptr, ProcPtr);
            typeAppend(code, bpptr);
            code.append(" = ");

            // Check if a number or a variable for higher level (grammar level)
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            code.append(" ");
            code.append(operation);
            code.append(" ");

            // Check if a number or a variable for lower level (grammar level)
            if (inherit_ptr == NULL)
            {
                code.append(inherit_str);
            }
            else
            {
                typeAppend(code, inherit_ptr);
            }

            emit(code);

            str = "";
            ptr = bpptr;
            MoreTerm(str, ptr, ProcPtr);
        }
    }
    // Else does nothing
}

/******************************************************************************
*** FUNCTION: Term(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)           ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Term grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Term(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements Term -> Factor MoreFactor
{
    Factor(str, ptr, ProcPtr);
    MoreFactor(str, ptr, ProcPtr);
}

/******************************************************************************
*** FUNCTION: MoreFactor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the MoreFactor gramr***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::MoreFactor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements MoreFactor -> mulopt Factor MoreFactor | e
{
    if (Token == mulopt)
    {
        if (Depth == 1) //------------Depth is equal to 1------------
        {
            string inherit_str = "";
            EntryPtr inherit_ptr = NULL;
            string code = "";
            string operation = Lexeme;

            Match(mulopt);
            Factor(inherit_str, inherit_ptr, ProcPtr);

            EntryPtr tempptr = newTemp();
            code.append(tempptr->Lexeme);
            code.append(" = ");

            // Check if a number or a variable for higher level (grammar level)
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            code.append(" ");
            code.append(operation);
            code.append(" ");

            // Check if a number or a variable for lower level (grammar level)
            if (inherit_ptr == NULL)
            {
                code.append(inherit_str);
            }
            else
            {
                typeAppend(code, inherit_ptr);
            }

            emit(code);

            str = "";
            ptr = tempptr;
            MoreFactor(str, ptr, ProcPtr);
        }
        else //------------Depth is greater than or equal to 2------------
        {
            string inherit_str = "";
            EntryPtr inherit_ptr = NULL;
            string code = "";
            string operation = Lexeme;

            Match(mulopt);
            Factor(inherit_str, inherit_ptr, ProcPtr);

            EntryPtr bpptr = newBP(str, ptr, ProcPtr);
            typeAppend(code, bpptr);
            code.append(" = ");

            // Check if a number or a variable for higher level (grammar level)
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            code.append(" ");
            code.append(operation);
            code.append(" ");

            // Check if a number or a variable for lower level (grammar level)
            if (inherit_ptr == NULL)
            {
                code.append(inherit_str);
            }
            else
            {
                typeAppend(code, inherit_ptr);
            }

            emit(code);

            str = "";
            ptr = bpptr;
            MoreFactor(str, ptr, ProcPtr);
        }
    }
    // Else does nothing
}

/******************************************************************************
*** FUNCTION: Factor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr)         ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Factor grammar  ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : str, ptr, ProcPtr                                         ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Factor(string& str, EntryPtr& ptr, EntryPtr& ProcPtr) // Implements Factor -> idt | numt | ( Expr ) | nott Factor | signopt Factor
{
    if (Token == idt)
    {
        ptr = lookup(Lexeme);
        checkForUndecl();
        Match(idt);
    }
    else if (Token == numt)
    {
        str = Lexeme;
        Match(numt);
    }
    else if (Token == lparent)
    {
        Match(lparent);
        Expr(str, ptr, ProcPtr);
        Match(rparent);
    }
    else if (Token == nott)
    {
        if (Depth == 1) //------------Depth is equal to 1------------
        {
            string code = "";
            Match(nott);
            Factor(str, ptr, ProcPtr);

            EntryPtr tempptr = newTemp();
            code.append(tempptr->Lexeme);
            code.append(" = NOT ");
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            emit(code);

            ptr = tempptr;
        }
        else //------------Depth is greater than or equal to 2------------
        {
            string code = "";
            Match(nott);
            Factor(str, ptr, ProcPtr);

            EntryPtr bpptr = newBP(str, ptr, ProcPtr);
            typeAppend(code, bpptr);
            code.append(" = NOT ");
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            emit(code);

            ptr = bpptr;
        }
    }
    else if (Token == addopt && Lexeme != "OR") // else if signopt
    {
        if (Depth == 1) //------------Depth is equal to 1------------
        {
            string code = "";
            string operation = Lexeme;
            Match(signopt);
            Factor(str, ptr, ProcPtr);

            EntryPtr tempptr = newTemp();
            code.append(tempptr->Lexeme);
            code.append(" = ");
            code.append(operation);
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            emit(code);

            ptr = tempptr;
        }
        else //------------Depth is greater than or equal to 2------------
        {
            string code = "";
            string operation = Lexeme;
            Match(signopt);
            Factor(str, ptr, ProcPtr);

            EntryPtr bpptr = newBP(str, ptr, ProcPtr);
            typeAppend(code, bpptr);
            code.append(" = ");
            code.append(operation);
            if (ptr == NULL)
            {
                code.append(str);
            }
            else
            {
                typeAppend(code, ptr);
            }

            emit(code);

            ptr = bpptr;
        }
    }
    else
    {
        cout << "Error on line " << LineNo << ": expecting " << symbols[idt] << ", " << symbols[numt] << ", " << symbols[nott] << ", " << symbols[lparent] << ", or " << symbols[signopt] << " but found " << symbols[Token] << endl;
        exit(1);
    }

}

/******************************************************************************
*** FUNCTION: Vars(EntryPtr& ProcPtr, int& varTotal, int& varCount)         ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Vars grammar    ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : ProcPtr, varTotal, varCount                               ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Vars(EntryPtr& ProcPtr, int& varTotal, int& varCount) // Implements -> idt Vartail | e
{
    //cout << "Vars" << endl;
    if (Token == idt)
    {
        varCount++;
        VarType TypVal;
        EntryType EntryVal;
        EntryPtr ptr;

        checkForDups();
        insert(Lexeme, Token, Depth);
        ptr = lookup(Lexeme);

        Match(idt);
        VarTail(ProcPtr, TypVal, EntryVal, varTotal, varCount);
        ptr->TypeOfEntry = EntryVal;

        if (EntryVal == varEntry)
        {
            // Put in size and offset into linkedlist
            ptr->var.TypeOfVariable = TypVal;

            if (TypVal == charType)
            {
                ptr->var.size = 1;
                offset = offset + 1;
            }
            else if (TypVal == intType)
            {
                ptr->var.size = 2;
                offset = offset + 2;
            }
            else if (TypVal == floatType)
            {
                ptr->var.size = 4;
                offset = offset + 4;
            }

            ptr->var.Offset = offset - varTotal;
            ptr->var.typeOfMode = inMode;
            ProcPtr->procedure.LastOffset = offset;

        }
        else // If it's a constant entry
        {
            if (TypVal == intType)
            {
                ptr->constant.TypeOfConstant = intType;
                ptr->constant.Value = Value;
            }
            else
            {
                ptr->constant.TypeOfConstant = floatType;
                ptr->constant.ValueR = ValueR;
            }
        }
    }
}

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
void Parser::VarTail(EntryPtr& ProcPtr, VarType& TypVal, EntryType& EntryVal, int& varTotal, int& varCount) // Implements VarTail -> , idt Vartail | : Vartail2 ; Vars | : Vartail 2 ;
{
    //cout << "VarTail" << endl;
    if (Token == commat)
    {
        varCount++;
        EntryPtr ptr;

        Match(commat);
        checkForDups();
        insert(Lexeme, Token, Depth);
        ptr = lookup(Lexeme);

        Match(idt);
        VarTail(ProcPtr, TypVal, EntryVal, varTotal, varCount);
        ptr->TypeOfEntry = EntryVal;

        if (EntryVal == varEntry)
        {
            // Put in size and offset into linkedlist
            ptr->var.TypeOfVariable = TypVal;

            if (TypVal == charType)
            {
                ptr->var.size = 1;
                offset = offset + 1;
            }
            else if (TypVal == intType)
            {
                ptr->var.size = 2;
                offset = offset + 2;
            }
            else if (TypVal == floatType)
            {
                ptr->var.size = 4;
                offset = offset + 4;
            }

            ptr->var.Offset = offset - varTotal;
            ptr->var.typeOfMode = inMode;
            ProcPtr->procedure.LastOffset = offset;
        }
        else // If it's a constant entry
        {
            if (TypVal == intType)
            {
                ptr->constant.TypeOfConstant = intType;
                ptr->constant.Value = Value;
            }
            else
            {
                ptr->constant.TypeOfConstant = floatType;
                ptr->constant.ValueR = ValueR;
            }
        }
    }
    else if (Token == colont)
    {
        Match(colont);
        VarTail2(TypVal, EntryVal, varTotal, varCount);
        Match(semit);

        if (Token == idt)
        {
            Vars(ProcPtr, varTotal, varCount);
        }
    }
}

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
void Parser::VarTail2(VarType& TypVal, EntryType& EntryVal, int& varTotal, int& varCount) // Implements -> Type | const assignop Value
{
    if (Token == constantt)
    {
        EntryVal = constEntry;
        Match(constantt);
        Match(assignopt);
        ValueFunc(TypVal);
    }
    else
    {
        EntryVal = varEntry;
        Type(TypVal, varTotal, varCount);
    }
}

/******************************************************************************
*** FUNCTION: checkForDups()                                                ***
*******************************************************************************
*** DESCRIPTION : checks to see if there are duplicate tokens               ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::checkForDups()
{
    EntryPtr ptr;
    ptr = lookup(Lexeme);
    if (ptr != NULL && ptr->depth == Depth)
    {
        cout << "Error on line " << LineNo << ": duplicate idt - " << Lexeme << endl;
        exit(1);
    }
}

/******************************************************************************
*** FUNCTION: checkForUndecl()                                              ***
*******************************************************************************
*** DESCRIPTION : checks to see if there are undeclared tokens              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::checkForUndecl()
{
    EntryPtr ptr;
    ptr = lookup(Lexeme);
    if (ptr == NULL)
    {
        cout << "Error on line " << LineNo << ": undeclared variable - " << Lexeme << endl;
        exit(1);
    }
}

/******************************************************************************
*** FUNCTION: newTemp()                                                     ***
*******************************************************************************
*** DESCRIPTION : creates a temporary variable                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
EntryPtr Parser::newTemp()
{
    // Code from lecture
    EntryPtr tempptr = NULL;
    static int i = 1;
    string temp = "_t";
    temp.append(to_string(i++));
    insert(temp, idt, Depth);

    if (i > 99)
    {
        cout << "Temp overflow, call George!" << endl;
        exit(1);
    }

    tempptr = lookup(temp);
    tempptr->TypeOfEntry = varEntry;
    tempptr->var.typeOfMode = inMode; // Fixes the @ symbol - added different modes into symbol table

    return tempptr;
}

/******************************************************************************
*** FUNCTION: emit(string code)                                             ***
*******************************************************************************
*** DESCRIPTION : prints the TAC into a new file                            ***
*** INPUT ARGS : code                                                       ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::emit(string code)
{
    static ofstream outFile;
    if (!outFile.is_open())
    {
        outFile.open(file);
    }
    outFile << code << endl;
    //cout << code << endl;
}

/******************************************************************************
*** FUNCTION: newBP(string str, EntryPtr ptr, EntryPtr& ProcPtr)            ***
*******************************************************************************
*** DESCRIPTION : makes a new temporary variable for depth >= 2             ***
*** INPUT ARGS : str, ptr                                                   ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : ProcPtr                                                   ***
*** RETURN : N/A                                                            ***
******************************************************************************/
EntryPtr Parser::newBP(string str, EntryPtr ptr, EntryPtr& ProcPtr)
{
    EntryPtr bpptr = NULL;
    static int i = 1;
    string bp = "_bp";
    bp.append(to_string(i++));
    insert(bp, idt, Depth);

    if (i > 99)
    {
        cout << "Temp overflow, call George!" << endl;
        exit(1);
    }

    bpptr = lookup(bp);
    bpptr->TypeOfEntry = varEntry;
    bpptr->var.typeOfMode = inMode; //Fixes the @ symbol 

    if (ptr == NULL) //If Lexeme on right-hand side is a number
    {
        if (str.find('.') == -1)
        {
            bpptr->var.TypeOfVariable = intType;
            bpptr->var.size = 2;
        }
        else
        {
            bpptr->var.TypeOfVariable = floatType;
            bpptr->var.size = 4;
        }
    }
    else //If Lexeme on right-hand side is an entry in the symbol table
    {
        if (ptr->TypeOfEntry == varEntry)  //Check if ptr is varEntry
        {
            bpptr->var.TypeOfVariable = ptr->var.TypeOfVariable;
            bpptr->var.size = ptr->var.size;
        }
        else  //ptr is constEntry
        {
            if (ptr->constant.TypeOfConstant == intType) //ptr is int
            {
                bpptr->var.TypeOfVariable = intType;
                bpptr->var.size = 2;
            }
            else  //ptr is float
            {
                bpptr->var.TypeOfVariable = floatType;
                bpptr->var.size = 4;
            }
        }
    }

    //Update Offset of TempPtr
    bpptr->var.Offset = -1 * (ProcPtr->procedure.SizeOfLocal + 2);

    //Update ProcPtr's size of local variables
    ProcPtr->procedure.SizeOfLocal = ProcPtr->procedure.SizeOfLocal + bpptr->var.size;

    return bpptr;
}

/******************************************************************************
*** FUNCTION: typeAppend(string& code, EntryPtr ptr)                        ***
*******************************************************************************
*** DESCRIPTION : appends the correct TAC code for the given ptr            ***
*** INPUT ARGS : ptr                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : code                                                      ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::typeAppend(string& code, EntryPtr ptr)
{
    if (ptr->TypeOfEntry == varEntry) //check if ptr is varEntry
    {
        if (ptr->depth == 1)
        {
            if (ptr->TypeOfEntry == varEntry) //check if ptr is varEntry
            {
                code.append(ptr->Lexeme);
            }
            else //ptr is constEntry
            {
                if (ptr->constant.TypeOfConstant == intType)   //ptr is int
                {
                    code.append(to_string(ptr->constant.Value));
                }
                else  //ptr is float
                {
                    code.append(to_string(ptr->constant.ValueR));
                }
            }
        }
        else //------------Depth is greater than or equal to 2------------
        {
            if (ptr->var.typeOfMode != inMode)
            {
                code.append("@");
            }

            code.append("_bp");

            if (ptr->var.Offset >= 0) //If offset is positive number add plus sign
            {
                code.append("+");
                code.append(to_string(ptr->var.Offset));
            }
            else
            {
                code.append(to_string(ptr->var.Offset)); //No modifications needed negative sign used as subtraction
            }
        }
    }
    else //ptr is constEntry
    {
        if (ptr->constant.TypeOfConstant == intType)   //ptr is int
        {
            code.append(to_string(ptr->constant.Value));
        }
        else  //ptr is float
        {
            code.append(to_string(ptr->constant.ValueR));
        }
    }
}

/******************************************************************************
*** FUNCTION: ProcCall(EntryPtr Ptr)                                        ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the ProcCall grammar***
*** INPUT ARGS : ptr                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::ProcCall(EntryPtr Ptr) // Implements -> idt ( Params )
{
    EntryPtr idtptr = NULL;
    string code = "";
    idtptr = lookup(Lexeme);

    Match(idt);
    Match(lparent);
    Params(idtptr->procedure.ParamList);
    Match(rparent);

    code.append("Call ");
    code.append(idtptr->Lexeme);
    emit(code);
}

/******************************************************************************
*** FUNCTION: Params(ParamPtr ptr)                                          ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Params grammar  ***
*** INPUT ARGS : ptr                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Params(ParamPtr ptr) // Implements -> idt ParamsTail | num ParamsTail | e
{
    if (Token == idt)
    {
        EntryPtr idtptr = NULL;
        string code = "";
        code.append("push ");

        if (ptr->typeOfMode != inMode)
        {
            code.append("@");
        }

        idtptr = lookup(Lexeme);
        typeAppend(code, idtptr);
        Match(idt);
        emit(code);
        ParamsTail(ptr->Next);
    }

    else if (Token == numt)
    {
        string code = "";
        code.append("push ");
        code.append(Lexeme);
        Match(numt);
        emit(code);
        ParamsTail(ptr->Next);
    }
}

/******************************************************************************
*** FUNCTION: ParamsTail(ParamPtr ptr)                                      ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the ParamsTaill grmr***
*** INPUT ARGS : ptr                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::ParamsTail(ParamPtr ptr) // Implements -> , idt ParamsTail | , num ParamsTail | e
{
    if (Token == commat)
    {
        Match(commat);
        if (Token == idt)
        {
            EntryPtr idtptr = NULL;
            string code = "";
            code.append("push ");

            if (ptr->typeOfMode != inMode)
            {
                code.append("@");
            }
            idtptr = lookup(Lexeme);
            typeAppend(code, idtptr);
            Match(idt);
            emit(code);
            ParamsTail(ptr->Next);
        }

        else if (Token == numt)
        {
            string code = "";
            code.append("push ");
            code.append(Lexeme);
            Match(numt);
            emit(code);
            ParamsTail(ptr->Next);
        }
    }
}

/******************************************************************************
*** FUNCTION: IO_Stat()                                                     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the IOStat grammar  ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::IO_Stat() // Implements -> In_Stat | Out_Stat
{
    if (Token == gett)
    {
        In_Stat();
    }
    else //Token == Putt
    {
        Out_Stat();
    }
}

/******************************************************************************
*** FUNCTION:In_Stat()                                                      ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the In_Stat grammar ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::In_Stat() // Implements -> get ( Id_List )
{
    Match(gett);
    Match(lparent);
    Id_List();
    Match(rparent);
}

/******************************************************************************
*** FUNCTION:Id_List()                                                      ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Id_List grammar ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Id_List() // Implements -> idt Id_List_Tail
{
    string code = "";
    EntryPtr ptr = NULL;
    ptr = lookup(Lexeme);
    if (ptr->TypeOfEntry == literalEntry)
    {
        code.append("rds ");
        code.append(Lexeme);
    }
    else //ptr->TypeOfEntry == varEntry
    {
        code.append("rdi ");
        typeAppend(code, ptr);
    }
    emit(code);

    Match(idt);
    Id_List_Tail();
}

/******************************************************************************
*** FUNCTION:Id_List_Tail()                                                 ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Id_List_tail grm***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Id_List_Tail() // Implements -> , idt Id_List_Tail | e
{
    if (Token == commat)
    {
        Match(commat);

        string code = "";
        EntryPtr ptr = NULL;
        ptr = lookup(Lexeme);
        if (ptr->TypeOfEntry == literalEntry)
        {
            code.append("rds ");
            code.append(Lexeme);
        }
        else //ptr->TypeOfEntry == varEntry
        {
            code.append("rdi ");
            typeAppend(code, ptr);
        }
        emit(code);

        Match(idt);
        Id_List_Tail();
    }
}

/******************************************************************************
*** FUNCTION:Out_Stat()                                                     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Out_Stat grammar***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Out_Stat() // Implements -> put ( Write_List ) | putln ( Write_List )
{
    if (Token == putt)
    {
        Match(putt);
        Match(lparent);
        Write_List(putt);
        Match(rparent);
    }
    else //Token == putlnt
    {
        Match(putlnt);
        Match(lparent);
        Write_List(putlnt);
        Match(rparent);
    }
}

/******************************************************************************
*** FUNCTION: Write_List(Symbol opcode)                                     ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Write_List gramr***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Write_List(Symbol opcode) // Implements -> Write_Token Write_List_Tail
{
    string code = "";
    EntryPtr ptr = NULL;
    Write_Token(ptr);
    if (ptr->TypeOfEntry == literalEntry)
    {
        code.append("wrs ");
        code.append(ptr->Lexeme);
    }
    else //ptr->TypeOfEntry == varEntry
    {
        code.append("wri ");
        typeAppend(code, ptr);
    }

    emit(code);
    code = "";
    if (opcode == putlnt)
    {
        code.append("wrln");
        emit(code);
    }

    Write_List_Tail(opcode);
}

/******************************************************************************
*** FUNCTION: Write_List_Tail(Symbol opcode)                                ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Write_List_Tail gm*
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Write_List_Tail(Symbol opcode) // Implements -> , Write_Token Write_List_Tail | e
{
    if (Token == commat)
    {
        Match(commat);

        string code = "";
        EntryPtr ptr = NULL;
        Write_Token(ptr);
        if (ptr->TypeOfEntry == literalEntry)
        {
            code.append("wrs ");
            code.append(ptr->Lexeme);
        }
        else //ptr->TypeOfEntry == varEntry
        {
            code.append("wri ");
            typeAppend(code, ptr);
        }

        emit(code);
        code = "";
        if (opcode == putlnt)
        {
            code.append("wrln");
            emit(code);
        }

        Write_List_Tail(opcode);
    }
}

/******************************************************************************
*** FUNCTION: Write_Token(EntryPtr& ptr)                                    ***
*******************************************************************************
*** DESCRIPTION : checks to see if actual Token matches the Write_Token grmr***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
void Parser::Write_Token(EntryPtr& ptr) // Implements -> idt | numt | literal
{
    if (Token == idt)
    {
        ptr = lookup(Lexeme);
        Match(idt);
    }
    else if (Token == numt)
    {
        ptr = lookup(Lexeme);
        Match(numt);
    }
    else //Token == litralt
    {
        ptr = newLiteral(Lexeme);
        Match(literalt);
    }
}

/******************************************************************************
*** FUNCTION: newLiteral(string str)                                        ***
*******************************************************************************
*** DESCRIPTION : creates a temporary variable                              ***
*** INPUT ARGS : N/A                                                        ***
*** OUTPUT ARGS : N/A                                                       ***
*** IN/OUT ARGS : N/A                                                       ***
*** RETURN : N/A                                                            ***
******************************************************************************/
EntryPtr Parser::newLiteral(string str)
{
    // Code from lecture
    EntryPtr literalsptr = NULL;
    static int i = 0;
    string literals = "_S";
    literals.append(to_string(i++));
    insert(literals, literalt, 1);

    if (i > 99)
    {
        cout << "Temp overflow, call George!" << endl;
        exit(1);
    }

    literalsptr = lookup(literals);
    literalsptr->TypeOfEntry = literalEntry;

    int j;
    for (j = 0; j < str.size(); j++)
    {
        literalsptr->literals.str[j] = str[j];
    }

    literalsptr->literals.str[j] = '\0'; //'\0' is used to designate end of str

    return literalsptr;
}