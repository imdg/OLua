#include "ASuffixedExpr.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ASuffixedExpr, ASimpleExpr)

RTTI_END(ASuffixedExpr)


ASuffixedExpr::ASuffixedExpr()
    : ASimpleExpr()
{
}

ASuffixedExpr::~ASuffixedExpr()
{
}

bool ASuffixedExpr::IsLValue()
{
    return true;
}
EVisitStatus ASuffixedExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ASuffixedExpr::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

