#include "ANilableUnwrap.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ANilableUnwrap, ASimpleExpr)
    RTTI_MEMBER(TargetExpr)
RTTI_END(ANilableUnwrap)


ANilableUnwrap::ANilableUnwrap()
    : ASimpleExpr()
    
{
}

ANilableUnwrap::~ANilableUnwrap()
{
}

EVisitStatus ANilableUnwrap::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ANilableUnwrap::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = TargetExpr->Accept(Vis);
    RETURN_IF_STOP(Status);
        
    return Vis->EndVisit(SThis);

}

bool ANilableUnwrap::IsConstant()
{
    return TargetExpr->IsConstant();
}

}

