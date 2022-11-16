#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ABreak : public AStat
{
    DECLEAR_RTTI()
public:
    ABreak();
    virtual ~ABreak();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
