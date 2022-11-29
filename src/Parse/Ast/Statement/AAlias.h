#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ATypeIdentity;

class AAlias : public AStat
{
    DECLEAR_RTTI()
public:
    AAlias();
    virtual ~AAlias();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    
    SPtr<ATypeIdentity> TargetType;
    OLString           Name;
};

}
