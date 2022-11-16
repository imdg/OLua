#include "AForNum.h"
#include "AExpr.h"
#include "AVarDecl.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AForNum, ABlockStat)
    RTTI_MEMBER(VarDecl)
    RTTI_MEMBER(Begin)
    RTTI_MEMBER(End)
    RTTI_MEMBER(Step)
RTTI_END(AForNum)


AForNum::AForNum()
    : ABlockStat()
{
}

AForNum::~AForNum()
{
}

EVisitStatus AForNum::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AForNum::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = VarDecl->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = Begin->Accept(Vis);
    RETURN_BY_STATUS(Status);

    Status = End->Accept(Vis);
    RETURN_BY_STATUS(Status);

    if(Step != nullptr)
    {
        Status = Step->Accept(Vis);
        RETURN_BY_STATUS(Status);
    }

    Status = MainBlock->Accept(Vis);
    RETURN_IF_STOP(Status);

    return Vis->EndVisit(SThis);
}

}

