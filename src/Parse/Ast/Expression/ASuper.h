#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "APrimaryExpr.h"

namespace OL
{
class ASuper : public APrimaryExpr
{
    DECLEAR_RTTI()
public:
    ASuper();
    virtual ~ASuper();
    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

};

}
