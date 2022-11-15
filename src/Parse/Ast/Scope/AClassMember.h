#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"

namespace OL
{

class AModifier;
class AClassMember : public ABase
{
    DECLEAR_RTTI()
public:
    AClassMember();
    virtual ~AClassMember();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<AModifier> Modifier;
};

}
