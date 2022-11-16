#include "ARepeat.h"
#include "AExpr.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ARepeat, ABlockStat)
    RTTI_MEMBER(Condition)
RTTI_END(ARepeat)


ARepeat::ARepeat()
    : ABlockStat()
{
}

ARepeat::~ARepeat()
{
}

EVisitStatus ARepeat::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ARepeat::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = MainBlock->Accept(Vis);
    RETURN_IF_STOP(Status);

    if(Status != VS_ContinueParent)
    {
        Status = Condition->Accept(Vis);
        RETURN_BY_STATUS(Status);
    }
    
    return Vis->EndVisit(SThis);
}

}

