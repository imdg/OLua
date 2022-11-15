#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "APrimaryExpr.h"

namespace OL
{
class AVarRef : public APrimaryExpr
{
    DECLEAR_RTTI()
public:
    AVarRef();
    virtual ~AVarRef();
    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString VarName;
};

}
