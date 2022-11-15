#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AMethod.h"

namespace OL
{

class AClassContructor : public AMethod
{
    DECLEAR_RTTI()
public:
    AClassContructor();
    virtual ~AClassContructor();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Name;

};

}
