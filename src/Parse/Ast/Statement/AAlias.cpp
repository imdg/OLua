#include "AAlias.h"
#include "ATypeIdentity.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AAlias, AStat)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(TargetType)
RTTI_END(AAlias)


AAlias::AAlias()
    : AStat()
    
{
}

AAlias::~AAlias()
{
}

EVisitStatus AAlias::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AAlias::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = TargetType->Accept(Vis);
    RETURN_IF_STOP(Status);
        
    return Vis->EndVisit(SThis);
}



}

