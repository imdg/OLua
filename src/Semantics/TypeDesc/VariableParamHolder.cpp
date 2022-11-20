/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "VariableParamHolder.h"
#include "SymbolScope.h"
#include "CompileMsg.h"


namespace OL
{

RTTI_BEGIN_INHERITED(VariableParamHolder, TypeDescBase)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(ParamType, MF_External)
RTTI_END(VariableParamHolder)

VariableParamHolder::VariableParamHolder(SPtr<ABase> Node, SPtr<TypeDescBase> InParamType)
{
    DeclNode = Node;
    ParamType = InParamType;
    IsResolved = true;
}
VariableParamHolder::VariableParamHolder(SPtr<ABase> Node, OLString ParamTypeName)
{
    DeclNode = Node;
    UnresolvedTypeName = ParamTypeName;
    IsResolved = false;
}

ETypeValidation VariableParamHolder::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->Is<VariableParamHolder>())
    {
        return ParamType->ValidateConvert(Target.PtrAs<VariableParamHolder>()->ParamType.Lock(), IsExplict);
    }
    else
        return TCR_NoWay;
    
}

bool VariableParamHolder::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<VariableParamHolder>())
    {
        return ParamType->EqualsTo(Target);
    }

    return false;

}

OLString VariableParamHolder::ToString()
{
    OLString Ret;
    Ret.Append(T("... as "));
    Ret.Append(ParamType->ToString().CStr());
    return Ret;
}

bool VariableParamHolder::IsNilable()
{
    return true;
}

SPtr<TypeDescBase> VariableParamHolder::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    return nullptr;   
}

SPtr<TypeDescBase> VariableParamHolder::AcceptUniOp(EUniOp Op)
{
    return nullptr;  
}

void VariableParamHolder::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(UnresolvedTypeName == T("") || IsResolved)
        return;

    SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedTypeName, true);
    if(Found == nullptr)
    {
        if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
        {
            CM.Log(CMT_NoType, DeclNode->Line, UnresolvedTypeName.CStr());
        }
    }
    else
    {
        IsResolved = true;
    }
}


}