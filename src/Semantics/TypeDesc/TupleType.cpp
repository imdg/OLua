#include "TupleType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"


namespace OL
{

RTTI_BEGIN_INHERITED(TupleType, TypeDescBase)

RTTI_END(TupleType)



ETypeValidation TupleType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->Is<TupleType>())
        return EqualsTo(Target.Get()) ? TCR_OK : TCR_NoWay;
    else
        return Subtypes[0].Type->ValidateConvert(Target, IsExplict);
    
}

bool TupleType::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<TupleType>())
    {
        TupleType* TargetTuple = Target->As<TupleType>();
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