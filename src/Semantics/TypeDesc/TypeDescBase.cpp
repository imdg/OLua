/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TypeDescBase.h"
#include "ABase.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
namespace OL
{





RTTI_BEGIN(TypeDescBase)
    RTTI_MEMBER(DeclNode, MF_External)
    RTTI_MEMBER(Valid)
RTTI_END(TypeDescBase)

TypeDescBase::TypeDescBase() : DeclNode(nullptr), Valid(true)
{

}
TypeDescBase::~TypeDescBase()
{
    
}

SPtr<TypeDescBase> TypeDescBase::GetActualType()
{
    return SThis;
}

void TypeDescBase::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{

}

bool TypeDescBase::IsInt() { return false; }
bool TypeDescBase::IsFloat() { return false; }
bool TypeDescBase::IsString() { return false; }
bool TypeDescBase::IsAny() { return false; }
bool TypeDescBase::IsBool() { return false; }
bool TypeDescBase::IsNil() { return false; }
bool TypeDescBase::IsImplicitAny() { return false;}

OperatorResult TypeDescBase::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    return OperatorResult{nullptr, false};
}
SPtr<TypeDescBase> TypeDescBase::AcceptUniOp(EUniOp Op)
{
    return nullptr;
}

SPtr<TypeDescBase> TypeDescBase::DeduceLValueType(SPtr<SymbolScope> Scope)
{
    return SThis;
}
bool TypeDescBase::IsValid()
{
    return Valid;
}

OLString TypeDescBase::DefaultValueText()
{
    return T("{}");
}

bool TypeDescBase::IsRefType()
{
    return true;
}

// OperatorResult TypeDescBase::AcceptNilCoalesing(SPtr<TypeDescBase> Target, bool TargetNilable)
// {
//     if(TargetNilable)
//     {

//     }

// }

}