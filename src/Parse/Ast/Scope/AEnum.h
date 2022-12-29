#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AEnumItem;
class AAttribute;
class AEnum : public AStat
{
    DECLEAR_RTTI()
public:
    AEnum();
    virtual ~AEnum();

    virtual EVisitStatus Accept(Visitor* Vis);

    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Name;
    OLList<SPtr<AEnumItem>> Items;
    SPtr<AAttribute> Attrib;
};

}
