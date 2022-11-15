#pragma once
#include "Common.h"

namespace OL
{

class SymbolScope;
class SourceFile;
struct Declearation;
class TypeDescBase;

struct ImportedScopeInfo
{
    WPtr<SymbolScope> Scope;
    OLString          SourceName;
};
DECLEAR_STRUCT_RTTI(ImportedScopeInfo)

struct ImportedDeclSearch
{
    SPtr<Declearation> Decl;
    SPtr<SymbolScope>   DeclScope;
};
DECLEAR_STRUCT_RTTI(ImportedScopeInfo)

class ImportedScopeGroup
{
public:
    DECLEAR_RTTI();
    OLList< ImportedScopeInfo >    ScopeInfo;

    void RegisterImportedScope(SPtr<SourceFile> File);

    ImportedDeclSearch FindImportedDecl(OLString Name,  bool Vars, bool Types);
    SPtr<TypeDescBase>  FindImportedNamedType(OLString Name);

};



}