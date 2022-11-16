#include "AFuncBody.h"
#include "AVarDecl.h"
#include "ABlockStat.h"
#include "ABlock.h"
#include "ATypeIdentity.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AFuncBody, ABase)
    RTTI_MEMBER(ReturnType)
    RTTI_MEMBER(Params)
    RTTI_MEMBER(MainBlock)
    RTTI_MEMBER(IsDeclearOnly)
RTTI_END(AFuncBody)


AFuncBody::AFuncBody()
    : IsDeclearOnly(false), ABase()
{
}

AFuncBody::~AFuncBody()
{
}

EVisitStatus AFuncBody::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AFuncBody::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    for(int i = 0; i < Params.Count(); i ++)
    {
        Status = Params[i]->Accept(Vis);
        if(Status == VS_Stop)
            return VS_Stop;
        if(Status == VS_ContinueParent)
            break;
    }

    if (Status != VS_ContinueParent)
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
    if(Status != VS_ContinueParent && MainBlock != nullptr) // main block could be null for abstract methods
    {
        Status = MainBlock->Accept(Vis);
        RETURN_IF_STOP(Status)
    }

    return Vis->EndVisit(SThis);
}

}

