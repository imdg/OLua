#include "AParentheses.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AParentheses, APrimaryExpr)
    RTTI_MEMBER(Expr)
RTTI_END(AParentheses)


AParentheses::AParentheses()
    : APrimaryExpr()
{
}

AParentheses::~AParentheses()
{
}

EVisitStatus AParentheses::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AParentheses::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = Expr->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = Vis->EndVisit(SThis);
    RETURN_BY_STATUS(Status);

    return VS_Continue;
}

}

