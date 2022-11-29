#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AClassMember.h"

namespace OL
{
class AExpr;
class AVarDecl;
class AClassVar : public AClassMember
{
    DECLEAR_RTTI()
public:
    AClassVar();
    virtual ~AClassVar();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);


    OLList<SPtr<AVarDecl>> Decls;
    OLList<SPtr<AExpr>> Inits;
    
};

}
