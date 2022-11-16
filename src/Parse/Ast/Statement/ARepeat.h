#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AExpr;
class ARepeat : public ABlockStat
{
    DECLEAR_RTTI()
public:
    ARepeat();
    virtual ~ARepeat();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AExpr> Condition;
};

}
