#include "ALocal.h"
#include "AExpr.h"
#include "AVarDecl.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ALocal, AStat)
    RTTI_MEMBER(Decls)
    RTTI_MEMBER(Inits)
RTTI_END(ALocal)


ALocal::ALocal()
    : AStat()
{
}

ALocal::~ALocal()
{
}

EVisitStatus ALocal::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ALocal::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < Decls.Count(); i++)
    {
        Status = Decls[i]->Accept(Vis);
        RETURN_IF_STOP(Status);
    }

    if(Status != VS_ContinueParent)
    {
        for (int i = 0; i < Inits.Count(); i++)
        {
            Status = Inits[i]->Accept(Vis);
            RETURN_IF_STOP(Status);
        }
    }
    return Vis->EndVisit(SThis);
}

}

