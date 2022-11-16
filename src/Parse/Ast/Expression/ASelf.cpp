#include "ASelf.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ASelf, APrimaryExpr)

RTTI_END(ASelf)


ASelf::ASelf()
    : APrimaryExpr()
{
}

ASelf::~ASelf()
{
}

bool ASelf::IsLValue()
{
    return true;
}

EVisitStatus ASelf::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ASelf::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

