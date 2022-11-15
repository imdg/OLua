#include "ARoot.h"
#include "AStat.h"
#include "ABlock.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ARoot, ABase)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Block)
RTTI_END(ARoot)


ARoot::ARoot()
    : ABase()
{
}

ARoot::~ARoot()
{
}

EVisitStatus ARoot::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ARoot::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_IF_STOP(Status)

    
    Status = Block->Accept(Vis);

    return Vis->EndVisit(SThis);
}

}

