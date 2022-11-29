#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AExpr;
class AVarDecl;
class AForNum : public ABlockStat
{
    DECLEAR_RTTI()
public:
    AForNum();
    virtual ~AForNum();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AVarDecl> VarDecl;
    SPtr<AExpr> Begin;
    SPtr<AExpr> End;
    SPtr<AExpr> Step;
};

}
