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
    RTTI_MEMBER(IsResolved)
    RTTI_MEMBER(IsNilable)
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
    if(Target->ActuallyIs<VariableParamHolder>())
    {
        return ParamType->ValidateConvert(Target->ActuallyAs<VariableParamHolder>()->ParamType.Lock(), IsExplict);
    }
    else
        return TCR_NoWay;
    
}

bool VariableParamHolder::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<VariableParamHolder>())
    {
        return ParamType->EqualsTo(Target);
    }

    return false;

}

OLString VariableParamHolder::ToString(bool IsNilable)
{
    OLString Ret;
    Ret.Append(T("... as "));
    Ret.Append(ParamType->ToString(IsNilable).CStr());
    return Ret;
}
OperatorResult VariableParamHolder::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    return OperatorResult{nullptr, false};   
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