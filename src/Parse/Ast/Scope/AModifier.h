#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"

namespace OL
{
RTTI_ENUM(EAccessType,
    AT_Public,
    AT_Protected,
    AT_Private
)

class AModifier : public ABase
{
    DECLEAR_RTTI()
public:
    AModifier();
    virtual ~AModifier();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    EAccessType Access;
    bool IsConst;
    bool IsStatic;
    bool IsVirtual;
    bool IsAbstract;
    bool IsOverride;
    
};

}
