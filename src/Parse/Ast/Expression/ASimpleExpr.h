#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AExpr.h"

namespace OL
{
class ASimpleExpr : public AExpr
{
    DECLEAR_RTTI()
public:
    ASimpleExpr();
    virtual ~ASimpleExpr();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
