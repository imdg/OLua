/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ImportedScopeGroup.h"
#include "SymbolScope.h"
#include "TypeDescBase.h"
#include "SourceFile.h"


namespace OL
{
STRUCT_RTTI_BEGIN(ImportedScopeInfo)
    RTTI_MEMBER(Scope, MF_External)
    RTTI_MEMBER(SourceName)
STRUCT_RTTI_END(ImportedScopeInfo)

STRUCT_RTTI_BEGIN(ImportedDeclSearch)
    RTTI_STRUCT_MEMBER(Decl, Declearation, MF_External)
    RTTI_MEMBER(DeclScope, MF_External)
STRUCT_RTTI_END(ImportedDeclSearch)

RTTI_BEGIN(ImportedScopeGroup)
    RTTI_STRUCT_MEMBER(ScopeInfo, ImportedScopeInfo)
RTTI_END(ImportedScopeGroup)

void ImportedScopeGroup::RegisterImportedScope(SPtr<SourceFile> File)
{
    for(int i = 0; i < ScopeInfo.Count(); i++)
    {
        if(ScopeInfo[i].SourceName == File->FileName)
            return;
    }

    ImportedScopeInfo NewImport;
    NewImport.Scope = File->Symbols->Scopes[0];
    NewImport.SourceName = File->FileName;

    ScopeInfo.Add(NewImport);
}

ImportedDeclSearch ImportedScopeGroup::FindImportedDecl(OLString Name,  bool Vars, bool Types )
{
    for(int i = 0; i < ScopeInfo.Count(); i++)
    {
        SPtr<Declearation> Decl = ScopeInfo[i].Scope->FindDecl(Name, Vars, Types);
        if(Decl != nullptr)
            return ImportedDeclSearch{Decl, ScopeInfo[i].Scope.Lock()};
    }
    return ImportedDeclSearch{nullptr, nullptr};
}

SPtr<TypeDescBase>  ImportedScopeGroup::FindImportedNamedType(OLString Name)
{
    for(int i = 0; i < ScopeInfo.Count(); i++)
    {
        SPtr<TypeDescBase> TypeDesc = ScopeInfo[i].Scope->FindNamedType(Name, false);
        if(TypeDesc != nullptr)
            return TypeDesc;
    }
    return nullptr;
}


}