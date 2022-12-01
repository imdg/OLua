#include "AExpr.h"
#include "TypeDescBase.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AExpr, ABase)
    RTTI_MEMBER(ExprType, MF_External)
    RTTI_MEMBER(IsNilable)
    RTTI_MEMBER(UsedAsType, MF_External)
    RTTI_MEMBER(IsUsedAsNilable)
RTTI_END(AExpr)


AExpr::AExpr()
    : IsNilable(false), IsUsedAsNilable(false), ABase()
{
}

AExpr::~AExpr()
{
}

bool AExpr::IsLValue()
{
    return false;
}

EVisitStatus AExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AExpr::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

bool AExpr::IsConstant()
{
    return false;
}

}

