#include "ASimpleExpr.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ASimpleExpr, AExpr)

RTTI_END(ASimpleExpr)


ASimpleExpr::ASimpleExpr()
    : AExpr()
{
}

ASimpleExpr::~ASimpleExpr()
{
}

EVisitStatus ASimpleExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ASimpleExpr::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

