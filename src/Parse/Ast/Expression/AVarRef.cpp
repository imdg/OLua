#include "AVarRef.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AVarRef, APrimaryExpr)
    RTTI_MEMBER(VarName)
RTTI_END(AVarRef)


AVarRef::AVarRef()
    : APrimaryExpr()
{
}

AVarRef::~AVarRef()
{
}

bool AVarRef::IsLValue()
{
    return true;
}

EVisitStatus AVarRef::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AVarRef::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

