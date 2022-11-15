#include "ABlock.h"
#include "AExpr.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ABlock, ABase)
    RTTI_MEMBER(Stats)
RTTI_END(ABlock)


ABlock::ABlock()
    : ABase()
{


    
}

ABlock::~ABlock()
{
}

EVisitStatus ABlock::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ABlock::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < Stats.Count(); i++)
    {
        EVisitStatus Status = Stats[i]->Accept(Vis);
        if(Status == VS_Stop)
            return VS_Stop;
        if(Status != VS_Continue)
            break;
    }



   
    return Vis->EndVisit(SThis);
}

}

