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
#include "FuncSigniture.h"

namespace OL
{
struct Declearation;
class InterfaceType;
class DeclearAttributes;
struct InterfaceMember
{
    OLString Name;
    WPtr<FuncSigniture> Func;
    uint    Flags; // of ClassMemberFlags
    WPtr<InterfaceType> Owner;
    WPtr<Declearation> Decl;
    WPtr<DeclearAttributes> MemberAttrib;
};
DECLEAR_STRUCT_RTTI(InterfaceMember)

class ANormalMethod;
class AModifier;

class InterfaceType : public TypeDescBase
{
    DECLEAR_RTTI()
public:
    virtual ~InterfaceType();
    void AddMethod(SPtr<ANormalMethod> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM);
    void AddUnresolvedBase(OLString BaseName);
    void SetInsideScope(SPtr<SymbolScope> Scope);
    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);
    
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    bool IsBaseType(TypeDescBase* Base);

    virtual OLString ToString();
    virtual bool IsNilable();

    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    SPtr<TypeDescBase> AcceptBinOpOverride(EBinOp Op, SPtr<TypeDescBase> Target);
    SPtr<TypeDescBase> AccecptUniOpOverride(EUniOp Op);


    InterfaceMember* FindMember(OLString Name, bool IncludeBase);
    InterfaceMember* FindMemberByDeclNode(SPtr<ABase> Node, bool IncludeBase);

    bool ValidateMember(InterfaceMember& NewMember, CompileMsg& CM);

    OLList<InterfaceMember>     Members;
    OLList<WPtr<InterfaceType>> BaseInterfaces;
    OLList<UnresolvedBaseTypeInfo>            UnresolvedBase;
    WPtr<SymbolScope>           InsideScope;
    OLString                    Name;
    WPtr<DeclearAttributes>     InterfaceAttrib;

};
}