/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "MapType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "ABase.h"
#include "AMapType.h"
#include "ATypeIdentity.h"
#include "IntrinsicType.h"
namespace OL
{


RTTI_BEGIN_INHERITED(MapType, TypeDescBase)
    RTTI_MEMBER(KeyType, MF_External)
    RTTI_MEMBER(ValueType, MF_External)
    RTTI_MEMBER(UnresolvedKeyType)
    RTTI_MEMBER(UnresolvedValueType)
    RTTI_MEMBER(IsValueNilable)
RTTI_END(MapType)


void MapType::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(KeyType == nullptr && UnresolvedKeyType != T(""))
    {
        SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedKeyType, true);
        if(Found == nullptr)
        {
            if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
            {
                CM.Log(CMT_NoType, DeclNode->Line,  UnresolvedKeyType.CStr());
                KeyType = IntrinsicType::CreateFromRaw(IT_any);
            }
        } 
        else
        {
            KeyType = Found;
        }
    }

    if(ValueType == nullptr && UnresolvedValueType != T(""))
    {
        SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedValueType, true);
        if(Found == nullptr)
        {
            if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
            {
                CM.Log(CMT_NoType, DeclNode->Line,  UnresolvedValueType.CStr());
                ValueType = IntrinsicType::CreateFromRaw(IT_any);
            }
        }
        else
        {
            ValueType = Found;
        }
    }
}

void MapType::SetKeyType(SPtr<TypeDescBase> InType)
{
    KeyType = InType;
    UnresolvedKeyType = T("");
    //IsKeyNilable = IsNilable;
}

void MapType::SetKeyType(OLString UnresolvedName)
{
    KeyType = nullptr;
    UnresolvedKeyType = UnresolvedName;
    //IsKeyNilable = IsNilable;
}

void MapType::SetValueType(SPtr<TypeDescBase> InType, bool IsNilable)
{
    ValueType = InType;
    UnresolvedValueType = T("");
    IsValueNilable = IsNilable;
}

void MapType::SetValueType(OLString UnresolvedName, bool IsNilable)
{
    ValueType = nullptr;
    UnresolvedValueType = UnresolvedName;
    IsValueNilable = IsNilable;
}

bool MapType::IsKeyTypeDecl(ATypeIdentity* Node)
{
    SPtr<AMapType> MapNode = DeclNode.Lock().PtrAs<AMapType>();

    if(MapNode->KeyType != nullptr && MapNode->KeyType->As<ATypeIdentity>() == Node)
        return true;
    return false;
}

bool MapType::IsValueTypeDecl(ATypeIdentity* Node)
{
    if(DeclNode.Lock()->As<AMapType>()->ValueType->As<ATypeIdentity>() == Node)
        return true;
    return false;
}

ETypeValidation MapType::ValidateConvert(SPtr<TypeDescBase> Target)
{
    if (EqualsTo(Target))
        return TCR_OK;
    
    if(Target->ActuallyIs<MapType>())
    {
        SPtr<MapType> TargetMap = Target->ActuallyAs<MapType>();
        ETypeValidation KeyVali = KeyType->ValidateConvert(TargetMap->KeyType.Lock());
        ETypeValidation ValueVali = ValueType->ValidateConvert(TargetMap->ValueType.Lock());
        return MergeMulityValidation(KeyVali, ValueVali);
    }

    if (Target->ActuallyIs<IntrinsicType>() && Target->ActuallyAs<IntrinsicType>()->Type == IT_any)
        return TCR_OK;

    return TCR_NoWay;
}

bool MapType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<MapType>())
    {
        SPtr<MapType> TargetMap = Target->ActuallyAs<MapType>();
        if(IsValueNilable != TargetMap->IsValueNilable)
            return false;
        if(KeyType->EqualsTo(TargetMap->KeyType.Lock()) == false 
            || ValueType->EqualsTo(TargetMap->ValueType.Lock()) == false)
            return false;
        return true;
    }
    return false;
}

OLString MapType::ToString(bool IsNilable)
{
    OLString Ret;
    Ret.AppendF(T("<%s, %s>%s"), KeyType->ToString(false).CStr(), ValueType->ToString(IsValueNilable).CStr(), (IsNilable?T("?"):T("")));
    return Ret;
}

OperatorResult MapType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<MapType>() || Target->IsAny() || Target->IsNil()))
    {
        return OperatorResult{IntrinsicType::CreateFromRaw(IT_bool), false};
    }

    return OperatorResult{nullptr, false};

     
}

SPtr<TypeDescBase> MapType::AcceptUniOp(EUniOp Op)
{
    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    if(Op == UO_Length)
        return IntrinsicType::CreateFromRaw(IT_int);
        
    return nullptr;
}

}