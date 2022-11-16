#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "APrimaryExpr.h"

namespace OL
{
class ASelf : public APrimaryExpr
{
    DECLEAR_RTTI()
public:
    ASelf();
    virtual ~ASelf();
    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

};

}
