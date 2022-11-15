
#include "SymbolTable.h"
#include "SymbolScope.h"
#include "ABase.h"
#include "ABlock.h"
#include "AClass.h"
#include "AInterface.h"
namespace OL
{

RTTI_BEGIN(SymbolTable)
    RTTI_MEMBER(Scopes)
RTTI_END(SymbolTable)

void SymbolTable::AddSymbolScope(SPtr<SymbolScope> NewScope)
{
    Scopes.Add(NewScope);
}


SPtr<SymbolScope> SymbolTable::FindByBlock(ABlock* Node)
{
    for(int i = 0; i < Scopes.Count(); i++)
    {
        if(Scopes[i]->Block.Lock().Get() == Node)
            return Scopes[i];
    }
    return nullptr;
}

SPtr<SymbolScope> SymbolTable::FindByClass(AClass* Node)
{
    for(int i = 0; i < Scopes.Count(); i++)
    {
        if(Scopes[i]->Owner.Lock().Get() == Node->As<ABase>())
            return Scopes[i];
    }
    return nullptr;
}

SPtr<SymbolScope> SymbolTable::FindByInterface(AInterface* Node)
{
    for(int i = 0; i < Scopes.Count(); i++)
    {
        if(Scopes[i]->Owner.Lock().Get() == Node->As<ABase>())
            return Scopes[i];
    }
    return nullptr;
}

void SymbolTable::ResolveSymbolsAndTypes(CompileMsg& CM, ESymbolResolvePhase Phase)
{
    for(int i = 0; i < Scopes.Count(); i++)
    {
        Scopes[i]->ResolveTypes(CM, Phase);
    }

    SymbolScope::FinishUnresolvedSymbol(Scopes, CM, Phase);
}


}
