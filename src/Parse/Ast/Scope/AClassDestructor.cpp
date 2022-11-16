#include "AClassDestructor.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AClassDestructor, AMethod)

RTTI_END(AClassDestructor)


AClassDestructor::AClassDestructor()
    : AMethod()
{
}

AClassDestructor::~AClassDestructor()
{
}

EVisitStatus AClassDestructor::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AClassDestructor::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    //Status = VisitMainBlock(Vis);
    //RETURN_IF_STOP(Status);

    return Vis->EndVisit(SThis);
}

}

