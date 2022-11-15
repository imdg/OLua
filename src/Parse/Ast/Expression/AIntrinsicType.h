#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ATypeIdentity.h"

namespace OL
{
class AIntrinsicType : public ATypeIdentity
{
    DECLEAR_RTTI()
public:
    AIntrinsicType();
    virtual ~AIntrinsicType();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    EIntrinsicType Type;
    bool IsImplicitAny;
    

};

}
