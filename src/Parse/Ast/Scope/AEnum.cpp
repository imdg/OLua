#include "AEnum.h"
#include "AEnumItem.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AEnum, AStat)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Items)
RTTI_END(AEnum)


AEnum::AEnum()
    : AStat()
{
}

AEnum::~AEnum()
{
}
// #undef SThis
// #define SThis ThisPtr<ABase>()

EVisitStatus AEnum::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AEnum::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < Items.Count(); i++)
    {
        Status = Items[i]->Accept(Vis);
        RETURN_IF_STOP(Status);
    }

    return Vis->EndVisit(SThis);
}

}

