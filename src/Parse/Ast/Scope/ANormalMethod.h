#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AMethod.h"

namespace OL
{

class ANormalMethod : public AMethod
{
    DECLEAR_RTTI()
public:
    ANormalMethod();
    virtual ~ANormalMethod();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Name;
};

}
