#include "AColonCall.h"
#include "TypeDescBase.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AColonCall, ACall)
    RTTI_MEMBER(NameAfter)
    RTTI_MEMBER(ColonMemberType, MF_External)
RTTI_END(AColonCall)


AColonCall::AColonCall()
    : ACall()
{
}

AColonCall::~AColonCall()
{
}

EVisitStatus AColonCall::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AColonCall::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = Func->Accept(Vis);
    RETURN_IF_STOP(Status)

    if(Status != VS_ContinueParent)
    {
        for (int i = 0; i < Params.Count(); i++)
        {
            Status = Params[i]->Accept(Vis);
            RETURN_IF_STOP(Status)
            if (Status == VS_ContinueParent)
                break;
        }
    }
    
    return Vis->EndVisit(SThis);
}

}

