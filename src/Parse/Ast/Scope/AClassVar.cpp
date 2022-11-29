#include "AClassVar.h"
#include "AExpr.h"
#include "AVarDecl.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AClassVar, AClassMember)
    RTTI_MEMBER(Decls)
    RTTI_MEMBER(Inits)
RTTI_END(AClassVar)


AClassVar::AClassVar()
    : AClassMember()
{
}

AClassVar::~AClassVar()
{
}

EVisitStatus AClassVar::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
    
}

EVisitStatus AClassVar::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    for(int i = 0; i < Decls.Count(); i++)
    {
        Status = Decls[i]->Accept(Vis);
        if(Status != VS_Continue)
            break;
    }
    RETURN_IF_STOP(Status)

    if(Status == VS_Continue)
    {
        for(int i = 0; i < Inits.Count(); i++)
        {
            Status = Inits[i]->Accept(Vis);
            if(Status != VS_Continue)
                break;
        }
    }
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}

}

