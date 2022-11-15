#pragma once
#include "Common.h"
#include "VisitorBase.h"
#include "SymbolScope.h"
#include "SymbolTable.h"
#include "OLFunc.h"


namespace OL
{

class AClass;
class ABlock;
class AInterface;
class ClassType;
class FuncSigniture;
class ScopeVisitorBase : public RecursiveVisitor
{

public:
    ScopeVisitorBase(SymbolTable& InSymbolTable);

    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);


    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);

    SPtr<ClassType>    GetOuterClass();
    SPtr<FuncSigniture> GetOuterFunc();

    bool IterateOuterClass(OLFunc<bool(SPtr<SymbolScope>) > Callback);

    SymbolTable&        CurrSymbolTable;
    SPtr<SymbolScope>   CurrScope;
};



}