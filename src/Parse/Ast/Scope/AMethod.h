#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AClassMember.h"

namespace OL
{

class AStat;
class AFuncBody;
class AAttribute;
class AMethod : public AClassMember
{
    DECLEAR_RTTI()
public:
    AMethod();
    virtual ~AMethod();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AFuncBody> Body;
    SPtr<AAttribute> Attrib;
};

}
