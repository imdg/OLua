#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ADereferance.h"
#include "APrimaryExpr.h"
namespace OL
{
class ADotMember : public ADereferance
{
    DECLEAR_RTTI()
public:
    ADotMember();
    virtual ~ADotMember();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<APrimaryExpr>  Target;
    OLString            Field;
};

}
