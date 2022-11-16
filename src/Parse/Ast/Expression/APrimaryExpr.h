#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AExpr.h"

namespace OL
{
class APrimaryExpr : public AExpr
{
    DECLEAR_RTTI()
public:
    APrimaryExpr();
    virtual ~APrimaryExpr();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
