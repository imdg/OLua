#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AMethod.h"

namespace OL
{
class AClassDestructor : public AMethod
{
    DECLEAR_RTTI()
public:
    AClassDestructor();
    virtual ~AClassDestructor();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
