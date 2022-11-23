/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TupleType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"


namespace OL
{

RTTI_BEGIN_INHERITED(TupleType, TypeDescBase)

RTTI_END(TupleType)



ETypeValidation TupleType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->ActuallyIs<TupleType>())
        return EqualsTo(Target) ? TCR_OK : TCR_NoWay;
    else
        return Subtypes[0].Type->ValidateConvert(Target, IsExplict);
    
}

bool TupleType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<TupleType>())
    {
        SPtr<TupleType> TargetTuple = Target->ActuallyAs<TupleType>();
        if(Subtypes.Count() != TargetTuple->Subtypes.Count())
        {
            return false;
        }

        for(int i = 0; i < Subtypes.Count(); i++)
        {
            if(Subtypes[i].Type.Lock()->EqualsTo(Target) == false)
            {
                return false;
            }
        }
        return true;
    }

    return false;

}

OLString TupleType::ToString()
{
    OLString Ret;
    Ret.Append(T("Tuple("));
    for(int i = 0; i < Subtypes.Count(); i++)
    {
        if(i != 0)
            Ret.AppendF(T(", %s"), Subtypes[i].Type->ToString().CStr());
        else
            Ret.AppendF(T("%s"), Subtypes[i].Type->ToString().CStr());
    }   
    Ret.Append(T(")"));
    return Ret;
}

bool TupleType::IsNilable()
{
    return false;
}

SPtr<TypeDescBase> TupleType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    if(Subtypes.Count() == 0)
        return nullptr;
    return Subtypes[0].Type->AcceptBinOp(Op, Target);    
}

SPtr<TypeDescBase> TupleType::AcceptUniOp(EUniOp Op)
{
    if(Subtypes.Count() == 0)
        return nullptr;
    return Subtypes[0].Type->AcceptUniOp(Op);    
}


void TupleType::AddSubtype(SPtr<TypeDescBase> NewSubtype)
{
    Subtypes.Add(TupleSubtype{NewSubtype});
}

}