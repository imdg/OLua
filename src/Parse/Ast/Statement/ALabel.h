#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ALabel : public AStat
{
    DECLEAR_RTTI()
public:
    ALabel();
    virtual ~ALabel();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Name;
};

}
