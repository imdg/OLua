#include "AStat.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AStat, ABase)

RTTI_END(AStat)


AStat::AStat()
    : ABase()
{
}

AStat::~AStat()
{
}

EVisitStatus AStat::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AStat::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

