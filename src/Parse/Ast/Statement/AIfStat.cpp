#include "AIfStat.h"
#include "AExpr.h"
#include "ABlock.h"

namespace OL
{

STRUCT_RTTI_BEGIN(ElseIfBlockInfo)
    RTTI_MEMBER(Condition)
    RTTI_MEMBER(Block)
STRUCT_RTTI_END(ElseIfBlockInfo)

RTTI_BEGIN_INHERITED(AIfStat, ABlockStat)
    RTTI_MEMBER(MainCondition)
    RTTI_MEMBER(MainIfBlock)
    RTTI_STRUCT_MEMBER(ElseIfBlocks, ElseIfBlockInfo)
    RTTI_MEMBER(ElseBlock)
RTTI_END(AIfStat)


AIfStat::AIfStat()
    : ABlockStat()
{
}

AIfStat::~AIfStat()
{
}

EVisitStatus AIfStat::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AIfStat::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = MainCondition->Accept(Vis);
    RETURN_IF_STOP(Status);

    if(Status != VS_ContinueParent)
    {
        Status = MainIfBlock->Accept(Vis);
        RETURN_IF_STOP(Status);
    }

    if (Status != VS_ContinueParent)
    {
        for (int i = 0; i < ElseIfBlocks.Count(); i++)
        {
            Status = ElseIfBlocks[i].Condition->Accept(Vis);
            RETURN_BY_STATUS(Status);

            Status = ElseIfBlocks[i].Block->Accept(Vis);
            RETURN_IF_STOP(Status);

            if (Status == VS_ContinueParent)
                break;
            ;
        }
    }

    if(Status != VS_ContinueParent && ElseBlock != nullptr)
    {
        Status = ElseBlock->Accept(Vis);
        RETURN_IF_STOP(Status);
    }
    
    return Vis->EndVisit(SThis);
}

}

