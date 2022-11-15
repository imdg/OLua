#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ATypeIdentity.h"

namespace OL
{
class ANamedType : public ATypeIdentity
{
    DECLEAR_RTTI()
public:
    ANamedType();
    virtual ~ANamedType();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString TypeName;

};

}
