#include "AClass.h"
#include "AClassMember.h"
#include "ATypeIdentity.h"
#include "AAttribute.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AClass, AStat)
    RTTI_MEMBER(ClassName)
    RTTI_MEMBER(IsExternal)
    RTTI_MEMBER(BaseClasses)
    RTTI_MEMBER(Attrib)
    RTTI_MEMBER(ClassMember)
RTTI_END(AClass)


AClass::AClass()
    : AStat()
{
}

AClass::~AClass()
{
}

EVisitStatus AClass::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AClass::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_IF_STOP(Status)

    if(Attrib != nullptr)
    {
        Status = Attrib->Accept(Vis);
        RETURN_IF_STOP(Status)
    }

    if(Status == VS_Continue)
    {
        for(int i = 0; i < BaseClasses.Count(); i++)
        {
            Status = BaseClasses[i]->Accept(Vis);
            RETURN_IF_STOP(Status)

            if (Status != VS_Continue)
                break;
        }
    }

    if(Status == VS_Continue)
    {
        for (int i = 0; i < ClassMember.Count(); i++)
        {
            Status = ClassMember[i]->Accept(Vis);
            RETURN_IF_STOP(Status)

            if (Status != VS_Continue)
                break;
        }
    }

    return Vis->EndVisit(SThis);
}

bool AClass::IsBase(SPtr<ABase> Node)
{
    for(int i = 0; i < BaseClasses.Count(); i++)
    {
        if(BaseClasses[i]->As<ABase>() == Node.Get())
            return true;
    }
    return false;
}

}

