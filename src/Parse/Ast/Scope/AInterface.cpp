#include "AInterface.h"
#include "ANormalMethod.h"
#include "ATypeIdentity.h"
#include "AAttribute.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AInterface, AStat)
    RTTI_MEMBER(InterfaceName)
    RTTI_MEMBER(Attrib)
    RTTI_MEMBER(BaseInterfaces)
    RTTI_MEMBER(InterfaceMembers)
RTTI_END(AInterface)


AInterface::AInterface()
    : AStat()
{
}

AInterface::~AInterface()
{
}

//#undef SThis
//#define SThis ThisPtr<ABase>()

EVisitStatus AInterface::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AInterface::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = Attrib->Accept(Vis);
    RETURN_IF_STOP(Status)

    if(Status == VS_Continue)
    {
        for(int i = 0; i < BaseInterfaces.Count(); i++)
        {
            Status = BaseInterfaces[i]->Accept(Vis);
            RETURN_IF_STOP(Status)

            if (Status != VS_Continue)
                break;
        }
    }

    if(Status == VS_Continue)
    {
        for (int i = 0; i < InterfaceMembers.Count(); i++)
        {
            Status = InterfaceMembers[i]->Accept(Vis);
            RETURN_IF_STOP(Status)

            if (Status != VS_Continue)
                break;
        }
    }

    return Vis->EndVisit(SThis);
}

bool AInterface::IsBase(SPtr<ABase> Node)
{
    for(int i = 0; i < BaseInterfaces.Count(); i++)
    {
        if(BaseInterfaces[i]->As<ABase>() == Node.Get())
            return true;
    }
    return false;
}

}

