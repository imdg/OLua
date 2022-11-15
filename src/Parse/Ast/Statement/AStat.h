#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"

namespace OL
{
class AStat : public ABase
{
    DECLEAR_RTTI()
public:
    AStat();
    virtual ~AStat();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
