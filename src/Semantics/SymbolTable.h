/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/


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