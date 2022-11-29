#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AClassMember;
class ATypeIdentity;
class AAttribute;
class AClass : public AStat
{
    DECLEAR_RTTI()
public:
    AClass();
    virtual ~AClass();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    bool IsBase(SPtr<ABase> Node);

    OLString ClassName;
    bool    IsExternal;
    OLList<SPtr<ATypeIdentity> > BaseClasses;
    OLList<SPtr<AClassMember> > ClassMember;

    SPtr<AAttribute>       Attrib;
};

}
