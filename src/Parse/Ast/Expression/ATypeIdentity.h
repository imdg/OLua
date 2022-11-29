#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"

namespace OL
{
class ATypeIdentity : public ABase
{
    DECLEAR_RTTI()
public:
    ATypeIdentity();
    virtual ~ATypeIdentity();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    bool IsNilable;


};

}
