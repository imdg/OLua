#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ASuffixedExpr;
class AExpr;
class AAssignment : public AStat
{
    DECLEAR_RTTI()
public:
    AAssignment();
    virtual ~AAssignment();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AExpr> > LeftExprs;
    OLList<SPtr<AExpr> >   RightExprs;
};

}
