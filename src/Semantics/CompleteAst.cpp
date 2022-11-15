#include "Common.h"
#include "CompleteAst.h"
#include "ABase.h"

namespace OL
{

CompleteAst::CompleteAst()
{

}

EVisitStatus CompleteAst::Visit(SPtr<ABase> Node)
{
    if(ParentStack.Count() > 0)
        Node->Parent = ParentStack.Top();
    return VS_Continue;
}


EVisitStatus CompleteAst::BeginVisit(SPtr<ABase> Node)
{
    if(ParentStack.Count() > 0)
        Node->Parent = ParentStack.Top();
    
    ParentStack.Add(Node);
    return VS_Continue;
}

EVisitStatus CompleteAst::EndVisit(SPtr<ABase> Node)
{
    ParentStack.Pop();
    return VS_Continue;
}

}