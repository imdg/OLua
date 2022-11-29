#include "ALabel.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ALabel, AStat)
    RTTI_MEMBER(Name)
RTTI_END(ALabel)


ALabel::ALabel()
    : AStat()
{
}

ALabel::~ALabel()
{
}

EVisitStatus ALabel::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ALabel::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

