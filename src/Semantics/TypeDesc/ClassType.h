/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"
#include "OLFunc.h"
#include "CompileCommon.h"
namespace OL
{

class AClassMember;
class AClassVar;
class ANormalMethod;
class AClassContructor;
class AModifier;
class SymbolScope;
class FuncSigniture;
class AExpr;
class ATypeIdentity;
class DeclearAttributes;

RTTI_ENUM(EClassMemberType,
    Member_Variant,
    Member_Function,
    Member_Type
)


class ClassType;
struct Declearation;
struct ClassMemberDesc
{
    OLString Name;
    WPtr<TypeDescBase>  DeclTypeDesc;
    WPtr<ATypeIdentity> RawTypeInfo; // only for class var, return type of method is in FuncSigniture of DeclTypeDesc
    uint    Flags;
    EClassMemberType Type;
    WPtr<AExpr> Init;   // only for class var.
    WPtr<ClassType> Owner;
    WPtr<Declearation> Decl;
    WPtr<DeclearAttributes> MemberAttrib;

};
DECLEAR_STRUCT_RTTI(ClassMemberDesc)

class CompileMsg;
struct InterfaceMember;
struct FindMemberResult
{
    bool IsClassMember;
    union 
    {
        ClassMemberDesc* FromClass;
        InterfaceMember* FromInterface;
    };
    
};


class ClassType : public TypeDescBase
{
    DECLEAR_RTTI();
public:
    ClassType();
    virtual ~ClassType();
    void AddClassVar(OLString Name, SPtr<AModifier> Modifier, SPtr<ATypeIdentity> RawType, SPtr<AExpr> Init, SPtr<Declearation> Decl, CompileMsg& CM);
    void AddNormalMethod(SPtr<ANormalMethod> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM);
    void AddConstructor(SPtr<AClassContructor> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM);
    void AddUnresolvedBase(OLString BaseClassName);

    void TryAddDefaultContructor();

    uint CreateFlag(SPtr<AModifier> Modifier);

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    void SetInsideScope(SPtr<SymbolScope> Scope);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString();

    static bool ValidateMember(ClassMemberDesc& NewMember, CompileMsg& CM);
    

    bool IsBaseType(SPtr<TypeDescBase> Base);
    FindMemberResult FindMember(OLString Name, bool IncludeBase);

    FindMemberResult FindMemberByDeclNode(SPtr<ABase> Node, bool IncludeBase);

    void FindVirtualFunc(OLList<FindMemberResult>& OutList);

    void ForAllMembers(bool IncludeBase, OLFunc<bool(ClassMemberDesc&)> Callback);

    bool IsMethod(SPtr<FuncSigniture> Func);
    
    
    SPtr<ClassType> GetSuperClass();

    

    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    SPtr<TypeDescBase> AcceptBinOpOverride(EBinOp Op, SPtr<TypeDescBase> Target);
    SPtr<TypeDescBase> AccecptUniOpOverride(EUniOp Op);

    OLList<ClassMemberDesc>     Members;

    OLList<WPtr<TypeDescBase> >     BaseTypes;
    OLString                Name;
    OLString                UniqueName;
    OLList<UnresolvedBaseTypeInfo>        UnresolvedBase;

    WPtr<SymbolScope>       InsideScope;
    WPtr<DeclearAttributes>       ClassAttrib;
    int NestLevel;
    bool IsExternal;
};


}
