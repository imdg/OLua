#pragma once
#include "Common.h"
#include "ScopeVistorBase.h"
#include "SymbolTable.h"
#include "AstDecl.h"
#include "VisitStackHelper.h"

namespace OL
{

class CompileMsg;

class ContextValidator : public ScopeVisitorBase
{
public:
    ContextValidator(SymbolTable& InSymbolTable, CompileMsg& InCM);
    CompileMsg& CM;

    struct CodeBlockInfo
    {
        SPtr<ABase> Node;
        bool ReturnCalled;
        bool BaseCtorCalled;
    };

    VisitStackHelper<CodeBlockInfo> BlockStack;

    void CheckStatAfterReturn(SPtr<ABase> Node);



    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncBody> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);

    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);

        virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus BeginVisit(SPtr<AIfStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AIfStat> Node);

    virtual EVisitStatus BeginVisit(SPtr<AStat> Node);
    virtual EVisitStatus Visit(SPtr<AStat> Node);
    

    virtual EVisitStatus BeginVisit(SPtr<AReturn> Node);
    virtual EVisitStatus EndVisit(SPtr<AReturn> Node);

    virtual EVisitStatus BeginVisit(SPtr<ACallStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ACallStat> Node);


    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);
};

}