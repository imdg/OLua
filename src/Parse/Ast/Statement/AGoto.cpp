#include "AGoto.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AGoto, AStat)
    RTTI_MEMBER(Label)
RTTI_END(AGoto)


AGoto::AGoto()
    : AStat()
{
}

AGoto::~AGoto()
{
}

EVisitStatus AGoto::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AGoto::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

