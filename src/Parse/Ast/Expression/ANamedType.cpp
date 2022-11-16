#include "ANamedType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ANamedType, ATypeIdentity)

RTTI_END(ANamedType)


ANamedType::ANamedType()
    : ATypeIdentity()
{
}

ANamedType::~ANamedType()
{
}

EVisitStatus ANamedType::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ANamedType::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

