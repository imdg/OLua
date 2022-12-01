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

    // Type derived from the expression itself
    WPtr<TypeDescBase> ExprType;
    bool               IsNilable;

    // Type that the expression has been used as in the parent
    // Incomformity with ExprType usually means that some type conversion may be needed while code generation
    // Can be null, meaning the same with ExprType
    WPtr<TypeDescBase> UsedAsType;
    bool               IsUsedAsNilable;
};

}
