#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ACall;
class ACallStat : public AStat
{
    DECLEAR_RTTI()
public:
    ACallStat();
    virtual ~ACallStat();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<ACall> CallExpr;
};

}
