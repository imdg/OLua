#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ATypeIdentity.h"

namespace OL
{
class ATypeIdentity;

class AArrayType : public ATypeIdentity
{
    DECLEAR_RTTI()
public:
    AArrayType();
    virtual ~AArrayType();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<ATypeIdentity> ElemType;
};

}
