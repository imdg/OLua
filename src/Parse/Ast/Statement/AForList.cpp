#include "AForList.h"
#include "AExpr.h"
#include "AVarDecl.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AForList, ABlockStat)
    RTTI_MEMBER(VarList)
    RTTI_MEMBER(Iterator)
RTTI_END(AForList)


AForList::AForList()
    : ABlockStat()
{
}

AForList::~AForList()
{
}

EVisitStatus AForList::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AForList::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < VarList.Count(); i++)
    {
        Status = VarList[i]->Accept(Vis);
        RETURN_BY_STATUS(Status);
    }
    if(Status != VS_ContinueParent)
    {
        Status = Iterator->Accept(Vis);
        RETURN_BY_STATUS(Status);
    }


    if(Status != VS_ContinueParent)
    {
        Status = MainBlock->Accept(Vis);
        RETURN_IF_STOP(Status);
    }

    return Vis->EndVisit(SThis);
}

}

