#include "AVarDecl.h"
#include "ATypeIdentity.h"
namespace OL
{

RTTI_BEGIN_INHERITED(AVarDecl, AStat)
    RTTI_MEMBER(VarName)
    RTTI_MEMBER(IsConst)
    RTTI_MEMBER(IsVariableParam)
    RTTI_MEMBER(IsOptionalParam)
    RTTI_MEMBER(VarType)
RTTI_END(AVarDecl)


AVarDecl::AVarDecl()
    : IsConst(false), IsVariableParam(false), IsOptionalParam(false), AStat()
{
}

AVarDecl::~AVarDecl()
{
}

EVisitStatus AVarDecl::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AVarDecl::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status)

    Status = VarType->Accept(Vis);
    RETURN_IF_STOP(Status)

    return Vis->EndVisit(SThis);
}



}

