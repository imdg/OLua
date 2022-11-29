#include "AFuncType.h"
#include "AVarDecl.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AFuncType, ATypeIdentity)
    RTTI_MEMBER(Params)
    RTTI_MEMBER(ReturnType)
RTTI_END(AFuncType)


AFuncType::AFuncType()
    : ATypeIdentity()
{
}

AFuncType::~AFuncType()
{
}

EVisitStatus AFuncType::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AFuncType::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    for(int i = 0; i < Params.Count(); i++)
    {
        Status = Params[i]->Accept(Vis);
        if(Status == VS_Stop)
            return VS_Stop;
        if(Status == VS_ContinueParent)
            break;
    }

    if(Status != VS_ContinueParent)
    {
        for (int i = 0; i < ReturnType.Count(); i++)
        {
            Status = ReturnType[i]->Accept(Vis);
            if (Status == VS_Stop)
                return VS_Stop;
            if (Status == VS_ContinueParent)
                break;
        }
    }

    return Vis->EndVisit(SThis);
}

}

