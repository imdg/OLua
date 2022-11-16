#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{

class AStat;
class AFuncBody;
class AFuncExpr : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    AFuncExpr();
    virtual ~AFuncExpr();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    //virtual bool IsConstant();

    SPtr<AFuncBody> Body;

    
};

}
