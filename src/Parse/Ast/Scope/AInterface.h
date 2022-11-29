#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{

class ANormalMethod;
class ATypeIdentity;
class AAttribute;
class AInterface : public AStat
{
    DECLEAR_RTTI()
public:
    AInterface();
    virtual ~AInterface();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    bool IsBase(SPtr<ABase> Node);
    OLString InterfaceName;
    OLList<SPtr<ATypeIdentity> > BaseInterfaces;
    OLList<SPtr<ANormalMethod> > InterfaceMembers;
    SPtr<AAttribute>       Attrib;
};

}
