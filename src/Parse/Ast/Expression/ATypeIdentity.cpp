#include "ATypeIdentity.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ATypeIdentity, ABase)
    RTTI_MEMBER(IsNilable)
RTTI_END(ATypeIdentity)


ATypeIdentity::ATypeIdentity()
    : IsNilable(false), ABase()
{
}

ATypeIdentity::~ATypeIdentity()
{
}

EVisitStatus ATypeIdentity::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ATypeIdentity::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

