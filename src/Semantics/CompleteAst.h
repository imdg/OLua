#pragma once
#include "Common.h"
#include "VisitorBase.h"


namespace OL
{

class CompleteAst : public RecursiveVisitor
{
public:
    CompleteAst();
    virtual EVisitStatus Visit(SPtr<ABase> Node) override;
    virtual EVisitStatus BeginVisit(SPtr<ABase> Node) override;
    virtual EVisitStatus EndVisit(SPtr<ABase> Node) override;
    OLList<SPtr<ABase>> ParentStack;
};



}