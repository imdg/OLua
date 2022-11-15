#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{
class ASuffixedExpr : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    ASuffixedExpr();
    virtual ~ASuffixedExpr();

    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
