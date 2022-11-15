#include "ANormalCall.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ANormalCall, ACall)

RTTI_END(ANormalCall)


ANormalCall::ANormalCall()
    : ACall()
{
}

ANormalCall::~ANormalCall()
{
}

EVisitStatus ANormalCall::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ANormalCall::Accept(RecursiveVisitor* Vis)
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

