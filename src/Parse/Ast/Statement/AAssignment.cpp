#include "AAssignment.h"
#include "ASuffixedExpr.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AAssignment, AStat)
    RTTI_MEMBER(LeftExprs)
    RTTI_MEMBER(RightExprs)
RTTI_END(AAssignment)


AAssignment::AAssignment()
    : AStat()
{
}

AAssignment::~AAssignment()
{
}

EVisitStatus AAssignment::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AAssignment::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    for(int i = 0; i < LeftExprs.Count(); i++)
    {
        Status = LeftExprs[i]->Accept(Vis);
        if(Status == VS_Stop)
            return VS_Stop;
        if(Status == VS_ContinueParent)
            break;
    }

    if(Status != VS_ContinueParent)
    {
        for(int i = 0; i < RightExprs.Count(); i++)
        {
            Status = RightExprs[i]->Accept(Vis);
            if (Status == VS_Stop)
                return VS_Stop;
            if (Status == VS_ContinueParent)
                break;
        }
    }
    return Vis->EndVisit(SThis);
}

}

