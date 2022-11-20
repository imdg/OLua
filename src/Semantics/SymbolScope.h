/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "CompileCommon.h"
#include "AAttribute.h"
namespace OL
{

RTTI_ENUM(EScopeType,
    ST_Class,
    ST_Interface,
    ST_Function,
    ST_Statements,
    ST_Global
)
class ABase;
class ABlock;
class AVarRef;
class TypeDescBase;
class ATypeIdentity;
class AFuncType;
class CompileMsg;

class DeclearAttributes
{
public:
    void CreateAttributeSet(OLList<AttributeItem>& Src);
    bool HasItem(OLString Name);
    bool HasItem(OLString Name, EIntrinsicType ExpectType);
    EIntrinsicType GetType(OLString Name);

    OLString GetString(OLString Name);
    int GetInt(OLString Name);
    float GetFloat(OLString Name);
    bool GetBool(OLString Name);


    OLMap<OLString, AttributeItem> Items;
};
DECLEAR_STRUCT_RTTI(DeclearAttributes)


RTTI_ENUM(EDeclType,
    DT_Class,
    DT_Interface,
    DT_Enum,
    DT_Function,
    DT_Variant,
)
struct Declearation
{
    OLString                Name;

    // If this is a variable declearation with specified type, this is the type node
    // Eg. 'int' node in 'xxx as int'
    // Could be nullptr in a type declearation
    WPtr<ATypeIdentity>     ValueTypeNode;  
    
    // The declearing node
    // Includes: AFuncDef, AMethod, AClass, AInterface, AEnum, AVarDecl
    WPtr<ABase>             DeclNode;

    EDeclType               DeclType;

    //TypeDesc for a variant, or the TypeDesc for a type declearation itself
    WPtr<TypeDescBase>      ValueTypeDesc;

    // Attributes for class, function, method
    SPtr<DeclearAttributes> Attrib;

    // Declear a symbol in another scope, like: function Actor:Run() .... end
    bool                    IsExternal;     
    
    bool                    IsConst;

    // If this is a '...'
    bool                    IsVariableParam; 
};
DECLEAR_STRUCT_RTTI(Declearation)

RTTI_ENUM(ERefType,
    Ref_Local,
    Ref_LocalType,
    Ref_UpVal,
    Ref_UpType,
    Ref_Deref,
    Ref_Member,
    Ref_MemberType,
    Ref_ExternalVal,
    Ref_ExternalType,
    Ref_Unresolved,
    Ref_UnresolvedType
)

class SymbolScope;

struct Reference
{
    Reference();
    OLString            Name;

    // Note: Deref will not be resolved in the general symbol resolving phase
    //       They are resolved in TypeDerivation
    ERefType            Type;
    
    // The node referring to another symbol
    // Includes: 
    //    AClass/AInterface: referring to base classes/interfaces
    //    AVarRef: A name of other symbol
    //    ADotMember/AColonCall: Dereference a member
    //    All nodes those have ANamedType:
    WPtr<ABase>         RefNode;
    
    // This node does not refere to the the symbol, but its sub block does
    // Usually this is a block stat(AWhile, AIfStat, AFuncBody)
    WPtr<ABase>         IndirectRefNode;

    // The decl which the ref is referring to is in this scope
    WPtr<SymbolScope>   DeclScope;

    // The decl which the ref is referring to 
    WPtr<Declearation>  Decl;

    // The first reference in the UpVal/UpType chain
    WPtr<Reference>     RootRef; 

};
DECLEAR_STRUCT_RTTI(Reference)

class ImportedScopeGroup;

class SymbolScope
{
    DECLEAR_RTTI();
public:
    SymbolScope();

public:
    OLString                    ScopeName;

    
    EScopeType                  Type;
    WPtr<SymbolScope>           Parent;
    WPtr<ABase>                 Owner; // Direct owner of ABlock, like AForNum, AFuncBody
    WPtr<TypeDescBase>          OwnerTypeDesc;  // Avaliable when current scope is Class or Interface
    WPtr<ABlock>                Block; // ABlock node if there is. (AClass does not have ABlock)
    
    OLList<SPtr<Declearation>>  Decls;
    OLList<SPtr<Reference>>     Refs;
    OLList<SPtr<TypeDescBase>>  TypeDefs;

    SPtr<ImportedScopeGroup>    Imports;  //Scope from
    bool                        HasVariableParam;

    // Works for any types of Scope, search only in current scope
    SPtr<Declearation> FindDecl(OLString Name, bool Vars, bool Types);

    // Works for Class and Interface, can search in base classes
    SPtr<Declearation> FindDeclInType(OLString Name, bool Vars, bool Types);
    
    SPtr<Declearation> FindDeclByNode(ABase* Node);
    SPtr<Reference> FindRefByNode(ABase* Node);

    void    AddDecl(SPtr<Declearation> NewDecl, CompileMsg& CM);
    static bool    DeclIsType(SPtr<Declearation> Decl);

    SPtr<TypeDescBase> FindNamedType(OLString Name, bool Recursive);
    //SPtr<TypeDescBase> FindFuncSigniture(ABase* TypeNode, bool Recursive);

    SPtr<TypeDescBase> FindTypeByNode(ABase* Node, bool Recursive);
    void ResolveTypes(CompileMsg& CM, ESymbolResolvePhase Phase);
    
    void GenScopeName();
    
    void BindImport(SPtr<ImportedScopeGroup> InImportScope);
    static void AddSymbolRef(SPtr<ABase> Node, OLString Name, SPtr<SymbolScope> Scope, bool IsType);
    static void FinishUnresolvedSymbol(OLList<SPtr<SymbolScope>>& InSymbolScopeList, CompileMsg& CM, ESymbolResolvePhase Phase);
    
    static bool IsResolvedType(ERefType RefType);
    static bool IsResolvedVar(ERefType RefType);

    static ERefType RefTypeFromDecl(SPtr<Declearation> Decl, SPtr<SymbolScope> DeclScope, bool IsExternal);


};


}