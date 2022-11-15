#include "ATypeIdentity.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ATypeIdentity, ABase)

RTTI_END(ATypeIdentity)


ATypeIdentity::ATypeIdentity()
    : ABase()
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

