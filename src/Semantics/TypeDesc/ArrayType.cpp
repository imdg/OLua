/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ArrayType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "ATypeIdentity.h"
#include "AArrayType.h"
#include "IntrinsicType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ArrayType, TypeDescBase)
    RTTI_MEMBER(ElemType, MF_External)
    RTTI_MEMBER(UnresolvedElemType)
    RTTI_MEMBER(IsElemNilable)
RTTI_END(ArrayType)

ArrayType::~ArrayType()
{
    
}

void ArrayType::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(ElemType == nullptr && UnresolvedElemType != T(""))
    {
        SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedElemType, true);
        if(Found == nullptr)
        {
            if(Phase == SRP_GlobalVar || Phase == SRP_Standalone)
            {
                CM.Log(CMT_NoType, DeclNode->Line,  UnresolvedElemType.CStr());
                ElemType = IntrinsicType::CreateFromRaw(IT_any);
            }
            
        }
        else
        {
            ElemType = Found;
        }
    }
}

void ArrayType::SetElemType(SPtr<TypeDescBase> InElemType, bool IsNilable)
{
    ElemType = InElemType;
    UnresolvedElemType = T("");
    IsNilable = IsNilable;
}
void ArrayType::SetElemType(OLString UnresolvedName, bool IsNilable)
{
    ElemType = nullptr;
    UnresolvedElemType = UnresolvedName;
    IsNilable = IsNilable;
}

bool ArrayType::IsElemTypeDecl(ATypeIdentity* Node)
{
    if(DeclNode.Lock()->As<AArrayType>()->ElemType->As<ATypeIdentity>() == Node)
        return true;
    return false;
}

ETypeValidation ArrayType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(EqualsTo(Target))
        return TCR_OK;
    if(Target->ActuallyIs<IntrinsicType>())
    {
        SPtr<IntrinsicType> Intri = Target->ActuallyAs<IntrinsicType>();
        if(Intri->Type == IT_any)
            return TCR_OK;
    }
    return TCR_NoWay;
}

bool ArrayType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<ArrayType>() == false)
        return false;

    if(IsElemNilable !=  Target->ActuallyAs<ArrayType>()->IsElemNilable)
        return false;

    if(ElemType.Lock()->EqualsTo(Target->ActuallyAs<ArrayType>()->ElemType.Lock()))
    {
        return true;
    }

    return false;

}

OLString ArrayType::ToString(bool IsNilable)
{
    OLString Ret;
    Ret.AppendF(T("%s[]%s"), ElemType->ToString(IsElemNilable).CStr(), IsNilable?T("?"):T(""));
    return Ret;
}


OperatorResult ArrayType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<ArrayType>() || Target->IsAny() || Target->IsNil()))
    {
        return OperatorResult{IntrinsicType::CreateFromRaw(IT_bool), false};
    }

    return OperatorResult{nullptr, false};

     
}

SPtr<TypeDescBase> ArrayType::AcceptUniOp(EUniOp Op)
{
    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    if(Op == UO_Length)
        return IntrinsicType::CreateFromRaw(IT_int);

    return nullptr;
}

}