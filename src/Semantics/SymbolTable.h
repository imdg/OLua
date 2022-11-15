#pragma once
#include "Common.h"
#include "CompileMsg.h"
#include "CompileCommon.h"
namespace OL
{
class SymbolScope;
class ABase;
class ABlock;
class AClass;
class AInterface;



class SymbolTable
{
    DECLEAR_RTTI();
public:
    void AddSymbolScope(SPtr<SymbolScope> NewScope);
    SPtr<SymbolScope> FindByBlock(ABlock* Node);
    SPtr<SymbolScope> FindByClass(AClass* Node);
    SPtr<SymbolScope> FindByInterface(AInterface* Node);

    void ResolveSymbolsAndTypes(CompileMsg& CM, ESymbolResolvePhase Phase);
    OLList<SPtr<SymbolScope>> Scopes;
};


}