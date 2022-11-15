#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{

class AExpr;
class AVarDecl;
class AForList : public ABlockStat
{
    DECLEAR_RTTI()
public:
    AForList();
    virtual ~AForList();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AVarDecl> > VarList;
    SPtr<AExpr> Iterator;
};

}
