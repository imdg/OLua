#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AFuncBody;
class AExpr;
class AAttribute;
class AFuncDef : public AStat
{
    DECLEAR_RTTI()
public:
    AFuncDef();
    virtual ~AFuncDef();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

  
    SPtr<AExpr> Owner;
    OLString Name;
    bool NeedSelf;
    bool IsLocal;
    bool IsExtern;
    SPtr<AFuncBody> Body;
    SPtr<AAttribute> Attrib;

};

}
