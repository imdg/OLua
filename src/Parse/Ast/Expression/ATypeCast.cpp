#include "ATypeCast.h"
#include "ATypeIdentity.h"
namespace OL
{

RTTI_BEGIN_INHERITED(ATypeCast, ASimpleExpr)
    //RTTI_MEMBER(TypeName)
    RTTI_MEMBER(TypeInfo)
    RTTI_MEMBER(CastExpr)
RTTI_END(ATypeCast)


ATypeCast::ATypeCast()
    : ASimpleExpr()
    
{
}

ATypeCast::~ATypeCast()
{
}

EVisitStatus ATypeCast::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ATypeCast::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    Status = CastExpr->Accept(Vis);
    RETURN_IF_STOP(Status);

    Status = TypeInfo->Accept(Vis);
    RETURN_IF_STOP(Status);
        
    return Vis->EndVisit(SThis);
}

bool ATypeCast::IsConstant()
{
    return CastExpr->IsConstant();
}

}

