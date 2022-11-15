#include "AMethod.h"
#include "AStat.h"
#include "AFuncBody.h"
#include "AAttribute.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AMethod, AClassMember)
    RTTI_MEMBER(Attrib)
    RTTI_MEMBER(Body)
RTTI_END(AMethod)


AMethod::AMethod()
    : AClassMember()
{
}

AMethod::~AMethod()
{
}

EVisitStatus AMethod::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AMethod::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    if(Attrib != nullptr)
    {
        Status = Attrib->Accept(Vis);
        RETURN_IF_STOP(Status)
    }
    if(Status == VS_Continue)
    {
        Status = Body->Accept(Vis);
        RETURN_IF_STOP(Status)
    }
    return Vis->EndVisit(SThis);
}



}

