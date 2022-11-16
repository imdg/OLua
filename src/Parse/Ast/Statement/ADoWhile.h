#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"


namespace OL
{

class AExpr;
class ADoWhile : public ABlockStat
{
    DECLEAR_RTTI()
public:
    ADoWhile();
    virtual ~ADoWhile();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AExpr> Condition;
};

}
