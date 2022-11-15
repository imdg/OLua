#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"
//#include "TypeDescBase.h"

namespace OL
{
class TypeDescBase;
class AExpr : public ABase
{
    DECLEAR_RTTI()
public:
    AExpr();
    virtual ~AExpr();
    virtual bool IsLValue();
    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    virtual bool IsConstant();
    WPtr<TypeDescBase> ExprType;
};

}
