#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASuffixedExpr.h"

namespace OL
{
class ADereferance : public ASuffixedExpr
{
    DECLEAR_RTTI()
public:
    ADereferance();
    virtual ~ADereferance();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
