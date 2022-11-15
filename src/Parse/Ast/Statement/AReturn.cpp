#include "AReturn.h"
#include "AExpr.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AReturn, AStat)
    RTTI_MEMBER(Ret)
RTTI_END(AReturn)


AReturn::AReturn()
    : AStat()
{
}

AReturn::~AReturn()
{
}

EVisitStatus AReturn::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AReturn::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < Ret.Count(); i++)
    {
        Status = Ret[i]->Accept(Vis);
        if(Status == VS_Stop)
            return VS_Stop;
        if(Status == VS_ContinueParent)
            break;
    }

    return Vis->EndVisit(SThis);
}

}

