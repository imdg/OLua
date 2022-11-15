#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{

class AExpr;
class AReturn : public AStat
{
    DECLEAR_RTTI()
public:
    AReturn();
    virtual ~AReturn();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AExpr> > Ret;
};

}
