#include "AModifier.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AModifier, ABase)
    RTTI_MEMBER(Access)
    RTTI_MEMBER(IsConst)
    RTTI_MEMBER(IsStatic)
    RTTI_MEMBER(IsVirtual)
    RTTI_MEMBER(IsAbstract)
    RTTI_MEMBER(IsOverride)
RTTI_END(AModifier)


AModifier::AModifier()
    : Access(AT_Public)
    , IsConst(false)
    , IsStatic(false)
    , IsVirtual(false)
    , IsAbstract(false)
    , IsOverride(false)
    , ABase()
{
}

AModifier::~AModifier()
{
}

EVisitStatus AModifier::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AModifier::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

