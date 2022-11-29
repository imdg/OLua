/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TypeDescBase.h"
#include "IntrinsicType.h"
#include "AIntrinsicType.h"
#include "EnumType.h"

namespace OL
{



RTTI_BEGIN_INHERITED(IntrinsicType, TypeDescBase)
    RTTI_MEMBER(Type)
RTTI_END(IntrinsicType)


SPtr<IntrinsicType> IntrinsicType::TypeInt;
SPtr<IntrinsicType> IntrinsicType::TypeFloat;
SPtr<IntrinsicType> IntrinsicType::TypeString;
SPtr<IntrinsicType> IntrinsicType::TypeBool;
SPtr<IntrinsicType> IntrinsicType::TypeAny;
SPtr<IntrinsicType> IntrinsicType::TypeImplicitAny;
SPtr<IntrinsicType> IntrinsicType::TypeNil;

IntrinsicType::IntrinsicType(EIntrinsicType InType) : Type(InType), IsImplicitAnyType(false)
{

}
IntrinsicType::IntrinsicType() : Type(IT_any), IsImplicitAnyType(false)
{

}

bool IntrinsicType::IsIntrinsic(ETypeCat InType)
{
    return InType == TC_Any || InType == TC_IntrinsicType;
}

SPtr<IntrinsicType> IntrinsicType::CreateFromRaw(EIntrinsicType RawType, bool InIsImplictAny)
{
    switch(RawType)
    {
        case IT_int:    return TypeInt      ? (TypeInt)     : (TypeInt      = new IntrinsicType(IT_int));
        case IT_float:  return TypeFloat    ? (TypeFloat)   : (TypeFloat    = new IntrinsicType(IT_float));
        case IT_string: return TypeString   ? (TypeString)  : (TypeString   = new IntrinsicType(IT_string));
        case IT_bool:   return TypeBool     ? (TypeBool)    : (TypeBool     = new IntrinsicType(IT_bool));
        case IT_nil:    return TypeNil      ? (TypeNil)     : (TypeNil      = new IntrinsicType(IT_nil));
        case IT_any:    
        {
            if(InIsImplictAny)
            {
                if(TypeImplicitAny == nullptr)
                {
                    TypeImplicitAny = new IntrinsicType(IT_any);
                    TypeImplicitAny->IsImplicitAnyType = true;
                }
                return TypeImplicitAny;
            }
            else
            {
                return TypeAny ? (TypeAny) : (TypeAny = new IntrinsicType(IT_any));
            }
        }

    }
    return nullptr;
}

ETypeValidation IntrinsicType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Type == IT_any)
        return TCR_OK;

    // some lua operation need to convert any type to nil
    // allow this as a comprimise (eg. forlist iterator)
    if(Type == IT_nil)
        return TCR_OK;

    if(Target->Is<IntrinsicType>())
    {
        IntrinsicType* TargetIntri = Target->As<IntrinsicType>();
        if(TargetIntri->Type == Type)
            return TCR_OK;

        if(TargetIntri->Type == IT_any)
            return TCR_OK;
        
        // int -> float
        if(Type == IT_int && TargetIntri->Type == IT_float)
            //return IsExplict ? TCR_OK : TCR_DataLose;
            return TCR_OK;

        // float -> int
        if(Type == IT_float && TargetIntri->Type == IT_int)
            return IsExplict ? TCR_OK : TCR_DataLose;

        // int -> bool
        if(Type == IT_int && TargetIntri->Type == IT_bool)
            return TCR_OK;

        // bool -> int
        if(Type == IT_bool && TargetIntri->Type == IT_int)
            return TCR_OK;

    }
    else if(Target->Is<EnumType>())
    {
        if(Type == IT_int)
            return IsExplict ? TCR_OK : TCR_Unsafe;
    }

    return TCR_NoWay;
}

bool IntrinsicType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<IntrinsicType>() && Target->ActuallyAs<IntrinsicType>()->Type == Type)
        return true;
    return false;
}

OLString InstrinsicTypeName(EIntrinsicType Type)
{
    switch (Type)
    {
    case IT_int:
        return T("int");
    case IT_float:
        return T("float");
    case IT_bool:
        return T("bool");
    case IT_string:
        return T("string");
    case IT_any:
        return T("any");
    case IT_nil:
        return T("nil");
    }
    return T("unknown");
}

OLString IntrinsicType::ToString(bool IsNilable)
{
    return InstrinsicTypeName(Type) + ((IsNilable && (Type != IT_nil)) ? T("?"):T(""));
}
bool IntrinsicType::IsInt() { return Type == IT_int; }
bool IntrinsicType::IsFloat() { return Type == IT_float; }
bool IntrinsicType::IsString() { return Type == IT_string; }
bool IntrinsicType::IsAny() { return Type == IT_any; }
bool IntrinsicType::IsBool() { return Type == IT_bool; }
bool IntrinsicType::IsNil() { return Type == IT_nil; }

bool IntrinsicType::IsImplicitAny() { return (Type == IT_any) && IsImplicitAnyType;}

struct IntrinsicOperation
{
    EIntrinsicType Op1;
    EIntrinsicType Op2;
    EIntrinsicType Result;
    bool Reversible;
};

IntrinsicOperation Op_AddSubMulPower[] ={
     {IT_int,   IT_int,     IT_int, true},
     {IT_int,   IT_float,   IT_float, true},
     {IT_int,   IT_any,     IT_int, true},
     {IT_float,   IT_float,   IT_float, true},
     {IT_float,   IT_any,     IT_float, true},
    {IT_any,   IT_any,     IT_any, true},

};

IntrinsicOperation Op_Div[] ={
     {IT_int,   IT_int,     IT_float, true},
     {IT_int,   IT_float,   IT_float, true},
     {IT_int,   IT_any,     IT_float, true},
     {IT_float,   IT_float,   IT_float, true},
     {IT_float,   IT_any,     IT_float, true},
     {IT_any,   IT_any,     IT_any, true},
};

IntrinsicOperation Op_IDiv[] ={
     {IT_int,   IT_int,     IT_int, true},
     {IT_int,   IT_float,   IT_int, true},
     {IT_int,   IT_any,     IT_int, true},
     {IT_float,   IT_float,   IT_int, true},
     {IT_float,   IT_any,     IT_int, true},
     {IT_any,   IT_any,     IT_any, true},
};

// BO_Greater BO_Less BO_GreaterEqual BO_LessEqual
IntrinsicOperation Op_AmountCompare[] ={
    {IT_int,    IT_int,     IT_bool, true},
    {IT_int,    IT_float,     IT_bool, true},
    {IT_float,    IT_float,     IT_bool, true},
    {IT_float,    IT_any,     IT_bool, true},
    {IT_int,    IT_any,     IT_bool, true},
};

// BO_And BO_Or
IntrinsicOperation Op_Logic[] ={
    
    {IT_int,    IT_int,     IT_bool, true},
    {IT_int,    IT_bool,     IT_bool, true},
    {IT_int,    IT_any,     IT_bool, true},
    {IT_int,    IT_nil,     IT_bool, true},

    {IT_bool,    IT_bool,     IT_bool, true},
    {IT_bool,    IT_any,     IT_bool, true},
    {IT_bool,    IT_nil,     IT_bool, true},
    
    {IT_any,    IT_any,     IT_bool, true},
    {IT_any,    IT_nil,     IT_bool, true},
    
    {IT_nil,    IT_nil,     IT_bool, true},
};

// BO_Equal BO_NotEqual
IntrinsicOperation Op_Eq[] ={
    {IT_int,  IT_int,       IT_bool, true},
    {IT_int,  IT_float,     IT_bool, true},
    {IT_int,  IT_bool,      IT_bool, true},
    {IT_int,  IT_any,      IT_bool, true},
    {IT_int,  IT_nil,      IT_bool, true},

    {IT_float,  IT_float,     IT_bool, true},
    {IT_float,  IT_bool,      IT_bool, true},
    {IT_float,  IT_any,      IT_bool, true},
    {IT_float,  IT_nil,      IT_bool, true},

    {IT_bool,  IT_bool,      IT_bool, true},
    {IT_bool,  IT_any,      IT_bool, true},
    {IT_bool,  IT_nil,      IT_bool, true},

    {IT_any,  IT_any,      IT_bool, true},
    {IT_any,  IT_nil,      IT_bool, true},

    {IT_nil,  IT_nil,      IT_bool, true},
    
    {IT_string, IT_string,  IT_bool, true},
    {IT_string, IT_nil,     IT_bool, true},
    {IT_string, IT_any,     IT_bool, true},
};

// BO_ShiftL BO_ShiftR
IntrinsicOperation Op_Shift[] ={
    {IT_int,  IT_int,       IT_int, true},
    {IT_any,   IT_any,     IT_any, true}
};

IntrinsicOperation Op_Cat[] ={
    {IT_string,  IT_string,       IT_string, true},
    {IT_string,  IT_any,       IT_string, true},
    {IT_any,   IT_any,     IT_any, true}
};

SPtr<TypeDescBase> AccecpBinOpWithIntrinsic(EIntrinsicType From, EIntrinsicType To, IntrinsicOperation* Table, int Size)
{
    for(int i = 0; i < Size; i++)
    {
        IntrinsicOperation& Curr = Table[i];
        if(Curr.Op1 == From && Curr.Op2 == To)
            return IntrinsicType::CreateFromRaw(Curr.Result);
        if(Curr.Reversible)
        {
            if(Curr.Op1 == To && Curr.Op2 == From)
                return IntrinsicType::CreateFromRaw(Curr.Result);
        }
    }
    return nullptr;
}

OperatorResult IntrinsicType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    if(Target->Is<IntrinsicType>() || Target->Is<EnumType>())
    {
        IntrinsicOperation* ConvertTable = nullptr;
        int TableSize = 0;
        EIntrinsicType TargetType = IT_int;
        if(Target->Is<IntrinsicType>())
        {
            TargetType = Target->As<IntrinsicType>()->Type;
        }
        bool IsResultNilable = TargetNilable;
        switch (Op)
        {
        case BO_Add: case BO_Sub: case BO_Mul: case BO_Power:
            ConvertTable = Op_AddSubMulPower;
            TableSize = sizeof(Op_AddSubMulPower) / sizeof(IntrinsicOperation);
            break;
        case BO_Div:
            ConvertTable = Op_Div;
            TableSize = sizeof(Op_Div) / sizeof(IntrinsicOperation);
            break;
        case BO_IDiv:
            ConvertTable = Op_IDiv;
            TableSize = sizeof(Op_IDiv) / sizeof(IntrinsicOperation);
            break;
        case BO_Cat:
            ConvertTable = Op_Cat;
            TableSize = sizeof(Op_Cat) / sizeof(IntrinsicOperation);
            break;            
        case BO_And: case BO_Or:
            ConvertTable = Op_Logic;
            TableSize = sizeof(Op_Logic) / sizeof(IntrinsicOperation);
            IsResultNilable = false;  // Logic operator returns non-nilable bool type,
            break;
        case BO_ShiftL: case BO_ShiftR:
            ConvertTable = Op_Shift;
            TableSize = sizeof(Op_Shift) / sizeof(IntrinsicOperation);
            break;
        case BO_Greater: case BO_Less: case BO_GreaterEqual: case BO_LessEqual:
            ConvertTable = Op_AmountCompare;
            TableSize = sizeof(Op_AmountCompare) / sizeof(IntrinsicOperation);
            IsResultNilable = false;  // Logic operator returns non-nilable bool type,
            break;
        case BO_Equal: case BO_NotEqual:
            ConvertTable = Op_Eq;
            TableSize = sizeof(Op_Eq) / sizeof(IntrinsicOperation);
            IsResultNilable = false;  // Logic operator returns non-nilable bool type,
            break;
        default:
            break;
        }

        if(ConvertTable != nullptr)
        {
            SPtr<TypeDescBase> ResultType = AccecpBinOpWithIntrinsic(Type, TargetType,  ConvertTable, TableSize);
            return OperatorResult{ResultType, IsResultNilable};
        }

        return OperatorResult{nullptr, false};
    }
    
    if(Type == IT_nil)
    {
        {
            return Target->AcceptBinOp(Op, SThis, TargetNilable);
        }
    }
    return OperatorResult{nullptr, false};;


}
SPtr<TypeDescBase> IntrinsicType::AcceptUniOp(EUniOp Op)
{
    switch(Op)
    {
        case UO_Minus: case UO_BNot:
        {
            if(Type == IT_int)      return IntrinsicType::CreateFromRaw(IT_int);
            if(Type == IT_float)    return IntrinsicType::CreateFromRaw(IT_int);
        }
        break;
        case UO_Not:
        {
            if(Type == IT_int)      return IntrinsicType::CreateFromRaw(IT_bool);
            if(Type == IT_bool)    return IntrinsicType::CreateFromRaw(IT_bool);
        }
        break;
    }
    return nullptr;
}

OLString IntrinsicType::DefaultValueText()
{
    switch (Type)
    {
    case IT_int:
        return T("0");
    case IT_float:
        return T("0.0");
    case IT_bool:
        return T("false");
    case IT_string:
        return T("\"\"");
    case IT_any:
        return T("0");
    case IT_nil:
        return T("nil");
    }
    return T("{}");
}

bool IntrinsicType::IsRefType()
{
    return false;
}


}