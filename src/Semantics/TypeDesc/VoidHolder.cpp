/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "VoidHolder.h"
#include "SymbolScope.h"
#include "CompileMsg.h"


namespace OL
{

RTTI_BEGIN_INHERITED(VoidHolder, TypeDescBase)

RTTI_END(VoidHolder)

SPtr<VoidHolder> VoidHolderInst;
SPtr<VoidHolder> VoidHolder::Get()
{
    if(VoidHolderInst == nullptr)
        VoidHolderInst = new VoidHolder();
    return VoidHolderInst;
}

ETypeValidation VoidHolder::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->Is<VoidHolder>())
        return TCR_OK;
    return TCR_NoWay;
}

bool VoidHolder::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<VoidHolder>())
        return true;

    return false;

}

OLString VoidHolder::ToString()
{
    OLString Ret;
    Ret.Append(T("void"));
    return Ret;
}

bool VoidHolder::IsNilable()
{
    return false;
}

SPtr<TypeDescBase> VoidHolder::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    return nullptr;    
}

SPtr<TypeDescBase> VoidHolder::AcceptUniOp(EUniOp Op)
{
    return nullptr;
}

}