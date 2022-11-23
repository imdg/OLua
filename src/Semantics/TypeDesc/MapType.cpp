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
}

void MapType::SetKeyType(OLString UnresolvedName)
{
    KeyType = nullptr;
    UnresolvedKeyType = UnresolvedName;
}

void MapType::SetValueType(SPtr<TypeDescBase> InType)
{
    ValueType = InType;
    UnresolvedValueType = T("");
}

void MapType::SetValueType(OLString UnresolvedName)
{
    ValueType = nullptr;
    UnresolvedValueType = UnresolvedName;
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

ETypeValidation MapType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if (EqualsTo(Target))
        return TCR_OK;

    if (Target->ActuallyIs<IntrinsicType>() && Target->ActuallyAs<IntrinsicType>()->Type == IT_any)
        return TCR_OK;

    return TCR_NoWay;
}

bool MapType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<MapType>())
    {
        SPtr<MapType> TargetMap = Target->ActuallyAs<MapType>();
        if(KeyType->EqualsTo(TargetMap->KeyType.Lock()) == false 
            || ValueType->EqualsTo(TargetMap->ValueType.Lock()) == false)
            return false;
        return true;
    }
    return false;
}

OLString MapType::ToString()
{
    OLString Ret;
    Ret.AppendF(T("<%s, %s>"), KeyType->ToString().CStr(), ValueType->ToString().CStr());
    return Ret;
}

bool MapType::IsNilable()
{
    return true;
}

SPtr<TypeDescBase> MapType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<MapType>() || Target->IsAny() || Target->IsNil()))
    {
        return IntrinsicType::CreateFromRaw(IT_bool);
    }

    return nullptr;

     
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