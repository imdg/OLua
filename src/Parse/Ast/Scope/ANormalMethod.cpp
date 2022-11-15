#include "ANormalMethod.h"

#include "AModifier.h"
#include "AFuncBody.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ANormalMethod, AMethod)
    RTTI_MEMBER(Name)
RTTI_END(ANormalMethod)


ANormalMethod::ANormalMethod()
    : AMethod()
{
}

ANormalMethod::~ANormalMethod()
{
}

EVisitStatus ANormalMethod::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ANormalMethod::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = Modifier->Accept(Vis);
    RETURN_IF_STOP(Status)
    
    if(Body != nullptr)
    {
        Status = Body->Accept(Vis);
        RETURN_IF_STOP(Status)
    }
    return Vis->EndVisit(SThis);
}

}

