#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "APrimaryExpr.h"

namespace OL
{
class AParentheses : public APrimaryExpr
{
    DECLEAR_RTTI()
public:
    AParentheses();
    virtual ~AParentheses();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AExpr> Expr;
};

}
