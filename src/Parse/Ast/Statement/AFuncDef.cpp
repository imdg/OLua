#include "AFuncDef.h"
#include "AFuncBody.h"
#include "AExpr.h"
#include "AAttribute.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AFuncDef, AStat)
    RTTI_MEMBER(Owner)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(NeedSelf)
    RTTI_MEMBER(IsLocal)
    RTTI_MEMBER(IsExtern)
    RTTI_MEMBER(Attrib)
    RTTI_MEMBER(Body)
RTTI_END(AFuncDef)


AFuncDef::AFuncDef()
    : NeedSelf(false), IsLocal(false), AStat()
{
}

AFuncDef::~AFuncDef()
{
}

EVisitStatus AFuncDef::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AFuncDef::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    if(Attrib != nullptr)
    {
        Status = Attrib->Accept(Vis);
        RETURN_IF_STOP(Status)
    }

    if(Status == VS_Continue && Owner != nullptr)
    {
        Status = Owner->Accept(Vis);
        RETURN_IF_STOP(Status);
    }
    
    if(Status == VS_Continue)
    {
        Status = Body->Accept(Vis);
        RETURN_IF_STOP(Status)
    }

    return Vis->EndVisit(SThis);
}

}

