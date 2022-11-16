#include "ADereferance.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ADereferance, ASuffixedExpr)

RTTI_END(ADereferance)


ADereferance::ADereferance()
    : ASuffixedExpr()
{
}

ADereferance::~ADereferance()
{
}

EVisitStatus ADereferance::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ADereferance::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

