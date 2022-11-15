#include "AConstructor.h"

namespace OL
{

STRUCT_RTTI_BEGIN(ConstructorElement)
    RTTI_MEMBER(Key)
    RTTI_MEMBER(Value)
STRUCT_RTTI_END(ConstructorElement)

RTTI_BEGIN_INHERITED(AConstructor, ASimpleExpr)
    RTTI_MEMBER(HasKey)
    RTTI_STRUCT_MEMBER(Elems, ConstructorElement)
RTTI_END(AConstructor)


AConstructor::AConstructor()
    : ASimpleExpr()
{
}

AConstructor::~AConstructor()
{
}

EVisitStatus AConstructor::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AConstructor::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < Elems.Count(); i++)
    {
        if(Status != VS_ContinueParent && Elems[i].Key != nullptr)
        {
            Status = Elems[i].Key->Accept(Vis);
            RETURN_IF_STOP(Status);
        }

        if (Status != VS_ContinueParent && Elems[i].Value != nullptr)
        {
            Status = Elems[i].Value->Accept(Vis);
            RETURN_IF_STOP(Status);
        }
    }
    return Vis->EndVisit(SThis);
}

bool AConstructor::IsConstant()
{
    return true;
}

void AConstructor::AddElement(AExpr* Key, AExpr* Value)
{
    ConstructorElement NewElem;
    NewElem.Key = Key;
    NewElem.Value = Value;

    Elems.Add(NewElem);
}

bool AConstructor::IsKey(ABase* Node)
{
    for(int i = 0; i < Elems.Count(); i++)
    {
        if(Elems[i].Key->As<ABase>() == Node)
            return true;
    }
    return false;
}

bool AConstructor::IsValue(ABase* Node)
{
    for(int i = 0; i < Elems.Count(); i++)
    {
        if(Elems[i].Value->As<ABase>() == Node)
            return true;
    }
    return false;
}

}

