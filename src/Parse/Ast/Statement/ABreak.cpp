#include "ABreak.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ABreak, AStat)

RTTI_END(ABreak)


ABreak::ABreak()
    : AStat()
{
}

ABreak::~ABreak()
{
}

EVisitStatus ABreak::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ABreak::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}



}

