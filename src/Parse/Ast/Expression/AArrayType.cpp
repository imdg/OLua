#include "AArrayType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AArrayType, ATypeIdentity)
    RTTI_MEMBER(ElemType)
RTTI_END(AArrayType)


AArrayType::AArrayType()
    : ATypeIdentity()
{
}

AArrayType::~AArrayType()
{
}

EVisitStatus AArrayType::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AArrayType::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = ElemType->Accept(Vis);
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}

}

