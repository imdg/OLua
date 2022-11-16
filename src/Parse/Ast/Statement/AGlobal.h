#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AVarDecl;
class AExpr;
class AGlobal : public AStat
{
    DECLEAR_RTTI()
public:
    AGlobal();
    virtual ~AGlobal();

    virtual EVisitStatus Accept(Visitor* Vis);

    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AVarDecl>> Decls;
    OLList<SPtr<AExpr>> Inits;
};

}
