#pragma once
#include "Common.h"
#include "VisitorBase.h"
#include "CompileCommon.h"

namespace OL
{


class ABase : public SThisOwner<ABase>
{
    DECLEAR_RTTI()
public:
    ABase();
    virtual ~ABase();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);


    bool IsParentOf(ABase* Child);
    CodeLineInfo Line;
    WPtr<ABase> Parent;
};
    




}