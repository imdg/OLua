#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{

struct ConstructorElement
{
    SPtr<AExpr> Key;
    SPtr<AExpr> Value;
};

class AConstructor : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    AConstructor();
    virtual ~AConstructor();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    virtual bool IsConstant();
    void AddElement(AExpr* Key, AExpr* Value);

    bool IsKey(ABase* Node);
    bool IsValue(ABase* Node);
    bool HasKey;
    OLList<ConstructorElement> Elems;

};

}
