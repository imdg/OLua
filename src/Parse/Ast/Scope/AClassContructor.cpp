#include "AClassContructor.h"

#include "AFuncBody.h"
#include "AModifier.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AClassContructor, AMethod)
    RTTI_MEMBER(Name)
RTTI_END(AClassContructor)


AClassContructor::AClassContructor()
    : AMethod()
{
}

AClassContructor::~AClassContructor()
{
}

EVisitStatus AClassContructor::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AClassContructor::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = Modifier->Accept(Vis);
    RETURN_IF_STOP(Status)
    
    Status = Body->Accept(Vis);
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}

}

