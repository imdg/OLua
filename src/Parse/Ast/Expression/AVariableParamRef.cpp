#include "AVariableParamRef.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AVariableParamRef, APrimaryExpr)
RTTI_END(AVariableParamRef)


AVariableParamRef::AVariableParamRef()
    : APrimaryExpr()
{
}

AVariableParamRef::~AVariableParamRef()
{
}

bool AVariableParamRef::IsLValue()
{
    return false;
}

EVisitStatus AVariableParamRef::Accept(Visitor* Vis)
{
    
    return Vis->Visit(SThis);
}

EVisitStatus AVariableParamRef::Accept(RecursiveVisitor* Vis)
{
    
    return Vis->Visit(SThis);
}

}

