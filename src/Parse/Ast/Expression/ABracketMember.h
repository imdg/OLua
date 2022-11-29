#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ADereferance.h"
#include "APrimaryExpr.h"
namespace OL
{
class ABracketMember : public ADereferance
{
    DECLEAR_RTTI()
public:
    ABracketMember();
    virtual ~ABracketMember();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<APrimaryExpr>  Target;
    SPtr<AExpr>         Field;
    
};

}
