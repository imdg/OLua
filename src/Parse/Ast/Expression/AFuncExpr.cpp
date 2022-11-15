#include "AFuncExpr.h"

#include "AStat.h"
#include "AFuncBody.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AFuncExpr, ASimpleExpr)
    RTTI_MEMBER(Body)
RTTI_END(AFuncExpr)


AFuncExpr::AFuncExpr()
    : ASimpleExpr()
{
}

AFuncExpr::~AFuncExpr()
{
}

EVisitStatus AFuncExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AFuncExpr::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = Body->Accept(Vis);
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}

// not consider as constant temperaryly
// to prevent duplication in multiple class construct

// bool AFuncExpr::IsConstant()
// {
//     return true;
// }

}

