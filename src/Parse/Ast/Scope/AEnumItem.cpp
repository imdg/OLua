#include "AEnumItem.h"
#include "AExpr.h"
#include "AVarDecl.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AEnumItem, ABase)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(ValueExpr)
RTTI_END(AEnumItem)


AEnumItem::AEnumItem()
    : ABase()
{
}

AEnumItem::~AEnumItem()
{
}

EVisitStatus AEnumItem::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AEnumItem::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    if(ValueExpr != nullptr)
    {
        Status = ValueExpr->Accept(Vis);
        RETURN_IF_STOP(Status);
    }
    
    return Vis->EndVisit(SThis);
}

}

