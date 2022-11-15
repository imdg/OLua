#include "APrimaryExpr.h"

namespace OL
{

RTTI_BEGIN_INHERITED(APrimaryExpr, AExpr)

RTTI_END(APrimaryExpr)


APrimaryExpr::APrimaryExpr()
    : AExpr()
{
}

APrimaryExpr::~APrimaryExpr()
{
}

EVisitStatus APrimaryExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus APrimaryExpr::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

