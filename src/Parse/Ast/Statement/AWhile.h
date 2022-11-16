#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AExpr;

class AWhile : public ABlockStat
{
    DECLEAR_RTTI()
public:
    AWhile();
    virtual ~AWhile();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AExpr> Condition;
};

}
