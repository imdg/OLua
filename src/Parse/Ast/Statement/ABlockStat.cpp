#include "ABlockStat.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ABlockStat, AStat)
    RTTI_MEMBER(MainBlock)
RTTI_END(ABlockStat)


ABlockStat::ABlockStat()
    : AStat()
{
}

ABlockStat::~ABlockStat()
{
}

EVisitStatus ABlockStat::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ABlockStat::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

// int ABlockStat::GetBlockCount()
// {
//     return 1;
// }

// OLList< SPtr<AStat> >& ABlockStat::GetBlock(int Index)
// {
//     OL_ASSERT(Index == 0);
//     return MainBlock;
// }




// EVisitStatus ABlockStat::VisitMainBlock(RecursiveVisitor* Vis)
// {
//     return VisitBlock(MainBlock, Vis);
// }

EVisitStatus ABlockStat::VisitBlock(OLList<SPtr<AStat> >& Block, RecursiveVisitor* Vis)
{
    for(int i = 0; i < Block.Count(); i++)
    {
        EVisitStatus Status = Block[i]->Accept(Vis);
        if(Status != VS_Continue)
            return Status;
    }

    return VS_Continue;

}


}

