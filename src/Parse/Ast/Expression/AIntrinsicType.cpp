#include "AIntrinsicType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AIntrinsicType, ATypeIdentity)
    RTTI_MEMBER(Type)
    RTTI_MEMBER(IsImplicitAny)
RTTI_END(AIntrinsicType)


AIntrinsicType::AIntrinsicType()
    : IsImplicitAny(false), ATypeIdentity()
{
}

AIntrinsicType::~AIntrinsicType()
{
}

EVisitStatus AIntrinsicType::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AIntrinsicType::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

