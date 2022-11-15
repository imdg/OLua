#include "AClassMember.h"
#include "AModifier.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AClassMember, ABase)
    RTTI_MEMBER(Modifier)
RTTI_END(AClassMember)


AClassMember::AClassMember()
    : ABase()
{
}

AClassMember::~AClassMember()
{
}

EVisitStatus AClassMember::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AClassMember::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}
