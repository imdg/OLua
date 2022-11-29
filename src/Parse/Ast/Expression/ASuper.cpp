#include "ASuper.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ASuper, APrimaryExpr)
RTTI_END(ASuper)


ASuper::ASuper()
    : APrimaryExpr()
{
}

ASuper::~ASuper()
{
}

bool ASuper::IsLValue()
{
    return true;
}

EVisitStatus ASuper::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ASuper::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

