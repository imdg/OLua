#include "ConstructorType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "ABase.h"
#include "AMapType.h"
#include "AArrayType.h"
#include "ArrayType.h"
#include "MapType.h"

#include "ATypeIdentity.h"
#include "AConstructor.h"

#include "IntrinsicType.h"
#include "VariableParamHolder.h"
namespace OL
{


RTTI_BEGIN_INHERITED(ConstructorType, TypeDescBase)

RTTI_END(ConstructorType)

ETypeValidation ConstructorType::ValidateIterator(SPtr<TypeDescBase> Type1, SPtr<TypeDescBase> Type2)
{
    SPtr<AConstructor> Ctor = DeclNode.Lock().PtrAs<AConstructor>();
    if(Ctor->HasKey == false)
    {
        ETypeValidation Ret = TCR_OK;
        for(int i = 0; i < Ctor->Elems.Count(); i++)
        {
            if(Ctor->Elems[i].Value.Get()->ExprType == nullptr)
                continue;
            
            ETypeValidation Curr = Ctor->Elems[i].Value.Get()->ExprType->ValidateConvert(Type1, false);
            if(Curr == TCR_NoWay)
                return TCR_NoWay;
            if(Curr == TCR_DataLose || Curr == TCR_Unsafe)
                Ret = Curr;
        }

        return Ret;
    }
    else
    {
        ETypeValidation Ret = TCR_OK;
        if(Type2 == nullptr)
            return TCR_NoWay;
            
        for (int i = 0; i < Ctor->Elems.Count(); i++)
        {
            ETypeValidation Curr = TCR_OK;
            if (Ctor->Elems[i].Key.Get()->ExprType != nullptr)
            {
                Curr = Ctor->Elems[i].Key.Get()->ExprType->ValidateConvert(Type1, false);
                if (Curr == TCR_NoWay)
                    return TCR_NoWay;
                if (Curr == TCR_DataLose || Curr == TCR_Unsafe)
                    Ret = Curr;
            }

            if (Ctor->Elems[i].Value.Get()->ExprType != nullptr)
            {
                Curr = Ctor->Elems[i].Value.Get()->ExprType->ValidateConvert(Type2, false);
                if (Curr == TCR_NoWay)
                    return TCR_NoWay;
                if (Curr == TCR_DataLose || Curr == TCR_Unsafe)
                    Ret = Curr;
            }
        }

        return Ret;
    }

    return TCR_OK;
}


ETypeValidation ConstructorType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    SPtr<AConstructor> Ctor = DeclNode.Lock().PtrAs<AConstructor>();
    if(Ctor->HasKey == false)
    {
        if(Target->Is<ArrayType>() == false)
            return TCR_NoWay;
        ETypeValidation Ret = TCR_OK;
        for(int i = 0; i < Ctor->Elems.Count(); i++)
        {
            if(Ctor->Elems[i].Value.Get()->ExprType == nullptr)
                continue;
            
            ETypeValidation Curr = Ctor->Elems[i].Value.Get()->ExprType->ValidateConvert(Target->As<ArrayType>()->ElemType.Lock(), IsExplict);
            if(Curr == TCR_NoWay)
                return TCR_NoWay;
            if(Curr == TCR_DataLose || Curr == TCR_Unsafe)
                Ret = Curr;
        }

        return Ret;
    }
    else
    {
        if(Target->Is<MapType>() == false)
            return TCR_NoWay;

        ETypeValidation Ret = TCR_OK;
        for (int i = 0; i < Ctor->Elems.Count(); i++)
        {
            ETypeValidation Curr = TCR_OK;
            if (Ctor->Elems[i].Key.Get()->ExprType != nullptr)
            {
                Curr = Ctor->Elems[i].Key.Get()->ExprType->ValidateConvert(Target->As<MapType>()->KeyType.Lock(), IsExplict);
                if (Curr == TCR_NoWay)
                    return TCR_NoWay;
                if (Curr == TCR_DataLose || Curr == TCR_Unsafe)
                    Ret = Curr;
            }

            if (Ctor->Elems[i].Value.Get()->ExprType != nullptr)
            {
                Curr = Ctor->Elems[i].Value.Get()->ExprType->ValidateConvert(Target->As<MapType>()->ValueType.Lock(), IsExplict);
                if (Curr == TCR_NoWay)
                    return TCR_NoWay;
                if (Curr == TCR_DataLose || Curr == TCR_Unsafe)
                    Ret = Curr;
            }
        }

        return Ret;
    }

    return TCR_OK;
}

SPtr<TypeDescBase> ConstructorType::DeduceLValueType(SPtr<SymbolScope> Scope)
{
    SPtr<AConstructor> Ctor = DeclNode.Lock().PtrAs<AConstructor>();
    if(Ctor->HasKey == false)
    {
        SPtr<ArrayType> Array = new ArrayType();
        Array->DeclNode = DeclNode;
        for(int i = 0; i < Ctor->Elems.Count(); i++)
        {
            SPtr<TypeDescBase> CurrElemType = Ctor->Elems[i].Value.Get()->ExprType.Lock();
            if(CurrElemType == nullptr)
                continue;

            if(CurrElemType->Is<VariableParamHolder>())
            {
                CurrElemType = CurrElemType.PtrAs<VariableParamHolder>()->ParamType.Lock();
            }

            if(Array->ElemType == nullptr)
            {
                Array->SetElemType(CurrElemType);
            }
            else
            {
                ETypeValidation Curr = CurrElemType->ValidateConvert(Array->ElemType.Lock(), false);
                if(Curr != TCR_OK)
                {
                    Array->SetElemType(IntrinsicType::CreateFromRaw(IT_any));
                }
            }
        }
        Scope->TypeDefs.Add(Array);
        Valid = false;
        return Array;
    }
    else
    {
        SPtr<MapType> Map = new MapType();
        Map->DeclNode = DeclNode;
        for(int i = 0; i < Ctor->Elems.Count(); i++)
        {
            if(Ctor->Elems[i].Key.Get()->ExprType == nullptr)
                continue;
            if(Map->KeyType == nullptr)
            {
                Map->SetKeyType(Ctor->Elems[i].Key.Get()->ExprType.Lock());
            }
            else
            {
                ETypeValidation Curr = Ctor->Elems[i].Key.Get()->ExprType->ValidateConvert(Map->KeyType.Lock(), false);
                if(Curr != TCR_OK)
                     Map->SetKeyType(IntrinsicType::CreateFromRaw(IT_any));
            }
        }

        for(int i = 0; i < Ctor->Elems.Count(); i++)
        {
            if(Ctor->Elems[i].Value.Get()->ExprType == nullptr)
                continue;
            if(Map->ValueType == nullptr)
            {
                Map->SetValueType(Ctor->Elems[i].Value.Get()->ExprType.Lock());
            }
            else
            {
                ETypeValidation Curr = Ctor->Elems[i].Value.Get()->ExprType->ValidateConvert(Map->ValueType.Lock(), false);
                if(Curr != TCR_OK)
                     Map->SetValueType(IntrinsicType::CreateFromRaw(IT_any));
            }
        }
        Scope->TypeDefs.Add(Map);
        Valid = false;
        return Map;   
    }
    return nullptr;
}

bool ConstructorType::EqualsTo(TypeDescBase* Target)
{
    return false;
}

OLString ConstructorType::ToString()
{
    OLString Ret;
    Ret.AppendF(T("(constructor)"));
    return Ret;
}

bool ConstructorType::IsNilable()
{
    return false;
}

}