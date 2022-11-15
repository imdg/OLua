#pragma once
#include "Common.h"
#include "VisitorBase.h"
#include "SymbolScope.h"
#include "SymbolTable.h"
#include "CompileMsg.h"

namespace OL
{

// SymbolWalker :
//      1. exploits all names in the AST, including variant/type declearation and references, called 'symbol'
//      2. creates scopes, and collect infomations of symbols of a scope
//      3. build as much relations between declearations and references. 
//         these relations, which is not complete, are still to be resolved in further steps

class SymbolWalker : public RecursiveVisitor
{
public:
    SymbolWalker(SymbolTable& InSymbolTable, CompileMsg& InCM);


    virtual EVisitStatus BeginVisit(SPtr<ABlockStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlockStat> Node);

    virtual EVisitStatus BeginVisit(SPtr<ARoot> Node);
    virtual EVisitStatus EndVisit(SPtr<ARoot> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<AMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<AMethod> Node);

    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);

    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus BeginVisit(SPtr<AEnum> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnum> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);

    virtual EVisitStatus BeginVisit(SPtr<AVarDecl> Node);

    virtual EVisitStatus Visit(SPtr<AVarRef> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADotMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<AColonCall> Node);

    virtual EVisitStatus BeginVisit(SPtr<ATypeCast> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);

    virtual EVisitStatus Visit(SPtr<ANamedType> Node);


    void    AddVarDecl(SPtr<AVarDecl> DeclNode);

    SymbolTable&               CurrSymbolTable;
    CompileMsg&                 CM;
    OLList<SPtr<SymbolScope>> WalkingStack;
    OLList<SPtr<ABase>>            OwnerStack;
    //OLList<SPtr<TypeDescBase>> TypeDefStack;
};

}