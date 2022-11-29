#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AVarDecl;
class AExpr;
class AEnumItem : public ABase
{
    DECLEAR_RTTI()
public:
    AEnumItem();
    virtual ~AEnumItem();

    virtual EVisitStatus Accept(Visitor* Vis);

    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

   OLString Name;
   SPtr<AExpr> ValueExpr;
};

}
