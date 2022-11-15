#include "EnumType.h"
#include "AEnum.h"
#include "AEnumItem.h"
#include "AExpr.h"
#include "IntrinsicType.h"

namespace OL
{
STRUCT_RTTI_BEGIN(EnumTypeItem)
    RTTI_MEMBER(ItemName)
    RTTI_MEMBER(ItemValue)
    RTTI_MEMBER(RawValueExpr, MF_External)
STRUCT_RTTI_END(EnumTypeItem)

RTTI_BEGIN(EnumType)
    RTTI_MEMBER(Name)
    RTTI_STRUCT_MEMBER(Items, EnumTypeItem)
RTTI_END(EnumType)

EnumType::~EnumType()
{
    
}

void EnumType::AddItem(SPtr<AEnumItem> Node)
{
    
    EnumTypeItem NewItem;
    NewItem.ItemName = Node->Name;
    NewItem.ItemValue = Items.Count() + 1;  
    NewItem.RawValueExpr = Node->ValueExpr;
    Items.Add(NewItem);
}   

ETypeValidation EnumType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(EqualsTo(Target.Get()))
        return TCR_OK;
    if(Target->Is<IntrinsicType>())
    {
        SPtr<IntrinsicType> Intri = Target.PtrAs<IntrinsicType>();
        if(Intri->Type == IT_any)
            return TCR_OK;
        if(Intri->Type == IT_int || Intri->Type == IT_float)
            return TCR_OK;
    }
    if(Target->Is<EnumType>()) // Not the same enum
    {
        return IsExplict ? TCR_OK : TCR_Unsafe;
    }
    return TCR_NoWay;
}

bool EnumType::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<EnumType>() && Target->DeclNode == DeclNode)
        return true;
    return false;
}
OLString EnumType::ToString()
{
    return Name;
}


bool EnumType::IsNilable()
{
    return false;
}

SPtr<TypeDescBase> EnumType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    return IntrinsicType::CreateFromRaw(IT_int)->AcceptBinOp(Op, Target);
}

SPtr<TypeDescBase> EnumType::AcceptUniOp(EUniOp Op)
{
    return IntrinsicType::CreateFromRaw(IT_int)->AcceptUniOp(Op);
}

bool EnumType::HasItem(OLString ItemName)
{
    for(int i = 0; i < Items.Count(); i++)
    {
        if(Items[i].ItemName == ItemName)
            return true;
    }
    return false;
}

int EnumType::GetItemValue(OLString Item)
{
    for(int i = 0; i < Items.Count(); i++)
    {
        if(Items[i].ItemName == Item)
            return Items[i].ItemValue;
    }
    return 0;
}


OLString EnumType::DefaultValueText()
{
    if(Items.Count() > 0)
    {
        OLString Ret;
        Ret.Printf(T("%d"), Items[0].ItemValue);
        return Ret;
    }
    return T("0");
}

bool EnumType::IsRefType()
{
    return false;
}

}