#include "ABracketMember.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ABracketMember, ADereferance)
    RTTI_MEMBER(Target)
    RTTI_MEMBER(Field)
RTTI_END(ABracketMember)


ABracketMember::ABracketMember()
    : ADereferance()
{
}

ABracketMember::~ABracketMember()
{
}

EVisitStatus ABracketMember::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ABracketMember::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = Target->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = Field->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = Vis->EndVisit(SThis);
    RETURN_BY_STATUS(Status);

    return VS_Continue;
}

}

