#include "ACall.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ACall, ASuffixedExpr)
    RTTI_MEMBER(Func)
    RTTI_MEMBER(Params)
RTTI_END(ACall)


ACall::ACall()
    : ASuffixedExpr()
{
}

ACall::~ACall()
{
}

EVisitStatus ACall::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ACall::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

