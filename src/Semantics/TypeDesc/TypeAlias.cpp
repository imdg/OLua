/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TypeAlias.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "IntrinsicType.h"


namespace OL
{

RTTI_BEGIN_INHERITED(TypeAlias, TypeDescBase)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(ActualType, MF_External)
RTTI_END(TypeAlias)


TypeAlias::TypeAlias()
{

}

TypeAlias::~TypeAlias()
{

}

void TypeAlias::SetActualType(SPtr<TypeDescBase> Type)
{
    ActualType = Type;
}
void TypeAlias::SetActualTypeName(OLString TypeName)
{
    UnresolvedTypeName = TypeName;
}

SPtr<TypeDescBase> TypeAlias::GetActualType()
{
    if(ActualType == nullptr)
    {
        return IntrinsicType::CreateFromRaw(IT_any);
    }
    else
        return ActualType.Lock();
}

void TypeAlias::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(ActualType != nullptr)
        return;
    
    OL_ASSERT(UnresolvedTypeName != T(""));

    SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedTypeName, true);
    if(Found != nullptr)
    {
        ActualType = Found;
        return;
    }

    if(Phase == SRP_Standalone || Phase == SRP_GlobalVar)
    {
        CM.Log(CMT_NoAliasType, DeclNode->Line, UnresolvedTypeName.CStr());
        ActualType = IntrinsicType::CreateFromRaw(IT_any);
    }

}

ETypeValidation TypeAlias::ValidateConvert(SPtr<TypeDescBase> Target)
{
    if(ActualType == nullptr)
    {
        return TCR_NoWay;
    }

    return ActualType->ValidateConvert(Target);
}

bool TypeAlias::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->EqualsTo(Target);
}

OLString TypeAlias::ToString(bool IsNilable)
{
    OLString RetString;
    if(ActualType == nullptr)
    {
        RetString.Printf(T("alias%s:%s:(unresolved)"), Name.CStr(), (IsNilable?T("?"):T("")));
    }
    else
    {
        RetString.Printf(T("alias%s:%s:(%s)"), (IsNilable?T("?"):T("")), Name.CStr(), ActualType->ToString(IsNilable).CStr());
    }
    return RetString;
}


bool TypeAlias::IsInt()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsInt();
}

bool TypeAlias::IsFloat()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsFloat();
}

bool TypeAlias::IsString()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsString();
}

bool TypeAlias::IsAny()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsAny();
}

bool TypeAlias::IsBool()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsBool();
}

bool TypeAlias::IsNil()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsNil();
}

bool TypeAlias::IsImplicitAny()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsImplicitAny();
}

bool TypeAlias::IsRefType()
{
    if(ActualType == nullptr)
    {
        return false;
    }
    return ActualType->IsRefType();
}

OperatorResult TypeAlias::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    if(ActualType == nullptr)
    {
        return OperatorResult{nullptr, false};
    }
    return ActualType->AcceptBinOp(Op, Target, TargetNilable);
}

SPtr<TypeDescBase> TypeAlias::AcceptUniOp(EUniOp Op)
{
    if(ActualType == nullptr)
    {
        return nullptr;
    }
    return ActualType->AcceptUniOp(Op);
}


SPtr<TypeDescBase> TypeAlias::DeduceLValueType(SPtr<SymbolScope> Scope)
{
    if(ActualType == nullptr)
    {
        return nullptr;
    }
    return ActualType->DeduceLValueType(Scope);
}


OLString TypeAlias::DefaultValueText()
{
    if(ActualType == nullptr)
    {
        return T("0");
    }
    return ActualType->DefaultValueText();
}



}