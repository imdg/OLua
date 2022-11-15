#include "AWhile.h"
#include "AExpr.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AWhile, ABlockStat)
    RTTI_MEMBER(Condition)
RTTI_END(AWhile)


AWhile::AWhile()
    : ABlockStat()
{
}

AWhile::~AWhile()
{
}

EVisitStatus AWhile::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AWhile::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = Condition->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = MainBlock->Accept(Vis);
    RETURN_IF_STOP(Status);
    
    return Vis->EndVisit(SThis);
}

}

