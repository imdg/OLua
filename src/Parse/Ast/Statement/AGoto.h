#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AGoto : public AStat
{
    DECLEAR_RTTI()
public:
    AGoto();
    virtual ~AGoto();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Label;
};

}
