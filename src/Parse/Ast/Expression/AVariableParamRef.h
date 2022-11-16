#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "APrimaryExpr.h"

namespace OL
{
class AVariableParamRef : public APrimaryExpr
{
    DECLEAR_RTTI()
public:
    AVariableParamRef();
    virtual ~AVariableParamRef();
    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

};

}
