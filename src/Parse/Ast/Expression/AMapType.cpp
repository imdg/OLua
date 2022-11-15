#include "AMapType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AMapType, ATypeIdentity)
    RTTI_MEMBER(KeyType)
    RTTI_MEMBER(ValueType)
RTTI_END(AMapType)


AMapType::AMapType()
    : ATypeIdentity()
{
}

AMapType::~AMapType()
{
}

EVisitStatus AMapType::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AMapType::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    if (KeyType != nullptr)
    {
        Status = KeyType->Accept(Vis);
        RETURN_IF_STOP(Status)
    }

    if(Status == VS_Continue)
    {
        Status = ValueType->Accept(Vis);
        RETURN_IF_STOP(Status)
    }
    return Vis->EndVisit(SThis);
}

}

