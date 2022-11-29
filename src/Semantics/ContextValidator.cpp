/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ContextValidator.h"
#include "SymbolTable.h"
#include "CompileMsg.h"
#include "AstInclude.h"
#include "FuncSigniture.h"
#include "ClassType.h"

namespace OL
{
ContextValidator::ContextValidator(SymbolTable& InSymbolTable, CompileMsg& InCM)
    : CM(InCM), ScopeVisitorBase(InSymbolTable)
{

}
    
EVisitStatus ContextValidator::BeginVisit(SPtr<AFuncBody> Node)
{
    
    return VS_Continue;
}

EVisitStatus ContextValidator::EndVisit(SPtr<AFuncBody> Node)
{
    if(Node->IsDeclearOnly == false)
    {
        VisitStackConsume<CodeBlockInfo> Curr(BlockStack);
        if(Node->ReturnType.Count() > 0 && Curr.GetElemCount() > 0 && Curr.GetElem(0).ReturnCalled == false)
        {
            CM.Log(CMT_NoReturn, Node->Line);
        }
    }
    return VS_Continue;
}


EVisitStatus ContextValidator::BeginVisit(SPtr<ABlock> Node)
{
    BlockStack.Push(CodeBlockInfo{Node, false, false});
    return ScopeVisitorBase::BeginVisit(Node);
}

EVisitStatus ContextValidator::EndVisit(SPtr<ABlock> Node)
{
    VisitStackPop<CodeBlockInfo> Curr(BlockStack);
    // Leave as it is
    return ScopeVisitorBase::EndVisit(Node);
}

// Push a dummy CodeBlockInfo for AClass
EVisitStatus ContextValidator::BeginVisit(SPtr<AClass> Node)
{
    BlockStack.Push(CodeBlockInfo{Node, false, false});
    return ScopeVisitorBase::BeginVisit(Node);
}

EVisitStatus ContextValidator::EndVisit(SPtr<AClass> Node)
{
    VisitStackPop<CodeBlockInfo> Curr(BlockStack);
    // Leave as it is
    return ScopeVisitorBase::EndVisit(Node);
}

// Push a dummy CodeBlockInfo for AInterface
EVisitStatus ContextValidator::BeginVisit(SPtr<AInterface> Node)
{
    BlockStack.Push(CodeBlockInfo{Node, false, false});
    return ScopeVisitorBase::BeginVisit(Node);
}

EVisitStatus ContextValidator::EndVisit(SPtr<AInterface> Node)
{
    VisitStackPop<CodeBlockInfo> Curr(BlockStack);
    // Leave as it is
    return ScopeVisitorBase::EndVisit(Node);
}


EVisitStatus ContextValidator::BeginVisit(SPtr<AReturn> Node)
{
    CheckStatAfterReturn(Node);
    BlockStack.Top().ReturnCalled = true;
    return VS_Continue;
}

EVisitStatus ContextValidator::EndVisit(SPtr<AReturn> Node)
{
    return VS_Continue;
}


EVisitStatus ContextValidator::BeginVisit(SPtr<AIfStat> Node)
{
    CheckStatAfterReturn(Node);
    return VS_Continue;
}

EVisitStatus ContextValidator::EndVisit(SPtr<AIfStat> Node)
{
    VisitStackConsume<CodeBlockInfo> Curr(BlockStack);
    OL_ASSERT(Curr.GetElemCount() == (Node->ElseBlock == nullptr ? 0 : 1) + Node->ElseIfBlocks.Count() + 1 );

    bool AllHaveReturn = true;
    bool AllHaveBaseCtor = true;
    if(Node->ElseBlock == nullptr)
    {
        AllHaveReturn = false;
        AllHaveBaseCtor = false;
    }
    else
    {
        for(int i = 0; i < Curr.GetElemCount(); i++)
        {
            if(Curr.GetElem(i).ReturnCalled == false)
            {
                AllHaveReturn = false;
            }

            if(Curr.GetElem(i).BaseCtorCalled == false)
            {
                AllHaveBaseCtor = false;
            }
        }
    }

    if(AllHaveReturn == true)
    {
        Curr.GetHoldingElem().ReturnCalled = true;
    }

        if(AllHaveBaseCtor == true)
    {
        Curr.GetHoldingElem().BaseCtorCalled = true;
    }
    return VS_Continue;


}

EVisitStatus ContextValidator::BeginVisit(SPtr<ACallStat> Node)
{
    CheckStatAfterReturn(Node);
    return VS_Continue;
}


EVisitStatus ContextValidator::EndVisit(SPtr<ACallStat> Node)
{
    return VS_Continue;
}


EVisitStatus ContextValidator::BeginVisit(SPtr<AStat> Node)
{
    CheckStatAfterReturn(Node);
    return VS_Continue;
}


EVisitStatus ContextValidator::Visit(SPtr<AStat> Node)
{
    CheckStatAfterReturn(Node);
    return VS_Continue;
}

EVisitStatus ContextValidator::Visit(SPtr<AVariableParamRef> Node)
{
    SPtr<FuncSigniture> Func = GetOuterFunc();
    if(Func == nullptr || Func->HasVariableParam() == false)
    {
        CM.Log(CMT_NoVariableParam, Node->Line);
        return VS_Stop;
    }
    return VS_Continue;
}

void ContextValidator::CheckStatAfterReturn(SPtr<ABase> Node)
{
    if(BlockStack.Top().ReturnCalled)
    {
        CM.Log(CMT_LastReturn, Node->Line);
    }
}


}