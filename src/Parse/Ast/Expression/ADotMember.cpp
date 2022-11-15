#include "ADotMember.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ADotMember, ADereferance)
    RTTI_MEMBER(Target)
    RTTI_MEMBER(Field)
RTTI_END(ADotMember)


ADotMember::ADotMember()
    : ADereferance()
{
}

ADotMember::~ADotMember()
{
}

EVisitStatus ADotMember::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ADotMember::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = Target->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = Vis->EndVisit(SThis);
    RETURN_BY_STATUS(Status);
    
    return VS_Continue;
}

}

