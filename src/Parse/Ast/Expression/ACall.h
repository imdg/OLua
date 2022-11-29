#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASuffixedExpr.h"

namespace OL
{
class ACall : public ASuffixedExpr
{
    DECLEAR_RTTI()
public:
    ACall();
    virtual ~ACall();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AExpr> Func;
    OLList<SPtr<AExpr>> Params;

};

}
