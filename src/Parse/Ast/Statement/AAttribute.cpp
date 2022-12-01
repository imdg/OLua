#include "AAttribute.h"

namespace OL
{

STRUCT_RTTI_BEGIN(AttributeItem)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Type)
    RTTI_MEMBER(IntVal)
    RTTI_MEMBER(FltVal)
    RTTI_MEMBER(BoolVal)
    RTTI_MEMBER(StrVal)
STRUCT_RTTI_END(AttributeItem)

RTTI_BEGIN_INHERITED(AAttribute, ABase)
    RTTI_STRUCT_MEMBER(Items, AttributeItem)
RTTI_END(AAttribute)


AAttribute::AAttribute()
    : ABase()
{
}

AAttribute::~AAttribute()
{
}

EVisitStatus AAttribute::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AAttribute::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

void AAttribute::AddString(OLString Name, OLString Val)
{
    AttributeItem& NewItem = Items.AddConstructed();
    NewItem.Name = Name;
    NewItem.Type = IT_string;
    NewItem.StrVal = Val;
}

void AAttribute::AddInt(OLString Name, int64 Val)
{
    AttributeItem& NewItem = Items.AddConstructed();
    NewItem.Name = Name;
    NewItem.Type = IT_int;
    NewItem.IntVal = Val;
}

void AAttribute::AddFloat(OLString Name, double Val)
{
    AttributeItem& NewItem = Items.AddConstructed();
    NewItem.Name = Name;
    NewItem.Type = IT_float;
    NewItem.FltVal = Val;
}

void AAttribute::AddBool(OLString Name, bool Val)
{
    AttributeItem& NewItem = Items.AddConstructed();
    NewItem.Name = Name;
    NewItem.Type = IT_bool;
    NewItem.BoolVal = Val;
}
void AAttribute::AddNil(OLString Name)
{
    AttributeItem& NewItem = Items.AddConstructed();
    NewItem.Name = Name;
    NewItem.Type = IT_nil;
}

}

