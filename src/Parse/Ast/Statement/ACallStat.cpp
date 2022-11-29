#include "ACallStat.h"
#include "ACall.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ACallStat, AStat)
    RTTI_MEMBER(CallExpr)
RTTI_END(ACallStat)


ACallStat::ACallStat()
    : AStat()
{
}

ACallStat::~ACallStat()
{
}

EVisitStatus ACallStat::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ACallStat::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = CallExpr->Accept(Vis);
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}

}

