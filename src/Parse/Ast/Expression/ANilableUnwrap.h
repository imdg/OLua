#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{


class ANilableUnwrap : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    ANilableUnwrap();
    virtual ~ANilableUnwrap();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    virtual bool IsConstant();
    

    SPtr<ASimpleExpr> TargetExpr;
};

}
