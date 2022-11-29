/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ScopeVistorBase.h"
#include "ABase.h"
#include "AClass.h"
#include "ABlock.h"
#include "ClassType.h"
#include "FuncSigniture.h"
namespace OL
{


ScopeVisitorBase::ScopeVisitorBase(SymbolTable& InSymbolTable) : CurrSymbolTable(InSymbolTable)
{

}

EVisitStatus ScopeVisitorBase::BeginVisit(SPtr<AClass> Node)
{
    CurrScope = CurrSymbolTable.FindByClass(Node.Get());
    return VS_Continue;
}

EVisitStatus ScopeVisitorBase::EndVisit(SPtr<AClass> Node)
{
    if(CurrScope)
    {
        CurrScope = CurrScope->Parent.Lock();
    }
    return VS_Continue;
}

EVisitStatus ScopeVisitorBase::BeginVisit(SPtr<AInterface> Node)
{
    CurrScope = CurrSymbolTable.FindByInterface(Node.Get());
    return VS_Continue;
}

EVisitStatus ScopeVisitorBase::EndVisit(SPtr<AInterface> Node)
{
    if(CurrScope)
    {
        CurrScope = CurrScope->Parent.Lock();
    }
    return VS_Continue;
}

EVisitStatus ScopeVisitorBase::BeginVisit(SPtr<ABlock> Node)
{
    CurrScope = CurrSymbolTable.FindByBlock(Node.Get());
    return VS_Continue;
}

EVisitStatus ScopeVisitorBase::EndVisit(SPtr<ABlock> Node)
{
    if(CurrScope)
    {
        CurrScope = CurrScope->Parent.Lock();
    }
    return VS_Continue;
}



SPtr<ClassType>    ScopeVisitorBase::GetOuterClass()
{
    SPtr<SymbolScope> SearchScope = CurrScope;
    while (SearchScope != nullptr)
    {
        if(SearchScope->Type == ST_Class)
        {
            OL_ASSERT(SearchScope->OwnerTypeDesc.Lock()->Is<ClassType>());
            return SearchScope->OwnerTypeDesc.Lock().PtrAs<ClassType>();
        }
        else if(SearchScope->Type == ST_Interface || SearchScope->Type == ST_Global)
        {
            return nullptr;
        }
        SearchScope = SearchScope->Parent.Lock();
    }
    return nullptr;
    
}

SPtr<FuncSigniture> ScopeVisitorBase::GetOuterFunc()
{
    SPtr<SymbolScope> SearchScope = CurrScope;
    while (SearchScope != nullptr)
    {
        if(SearchScope->Type == ST_Function)
        {
            ABase* ScopeOwner = SearchScope->Owner.Get();
            SymbolScope* Parent = SearchScope->Parent.Get();
            SPtr<TypeDescBase> ScopeType = Parent->FindTypeByNode(ScopeOwner, false);
            if(ScopeType->Is<FuncSigniture>())
                return ScopeType.PtrAs<FuncSigniture>();
            return nullptr;
        }
        else if(SearchScope->Type == ST_Class || SearchScope->Type == ST_Interface || SearchScope->Type == ST_Global)
        {
            return nullptr;
        }
        SearchScope = SearchScope->Parent.Lock();
    }
    return nullptr;
}


bool ScopeVisitorBase::IterateOuterClass(OLFunc<bool(SPtr<SymbolScope>) > Callback)
{
    SPtr<SymbolScope> Scope = CurrScope;
    while(Scope != nullptr)
    {
        if(Scope->Type == ST_Class)
        {
            if(false == Callback(Scope))
                return false;
        }
        Scope = Scope->Parent.Lock();
    }
    return true;
}
}