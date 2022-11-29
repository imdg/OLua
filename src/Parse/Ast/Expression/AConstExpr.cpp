#include "AConstExpr.h"
#include "AstPool.h"

namespace OL
{

RTTI_BEGIN_INHERITED(AConstExpr, ASimpleExpr)
    RTTI_MEMBER(Type)
    RTTI_MEMBER(IntVal)
    RTTI_MEMBER(FltVal)
    RTTI_MEMBER(BoolVal)
    RTTI_MEMBER(StrVal)
RTTI_END(AConstExpr)


AConstExpr::AConstExpr()
    : ASimpleExpr()
    , Type(IT_nil)
    , IntVal(0)
    , FltVal(0.0f)
    , BoolVal(false)
    , StrVal(T(""))
{
}

AConstExpr::~AConstExpr()
{
}

bool AConstExpr::IsConstant()
{
    return true;
}

AConstExpr* AConstExpr::NewInt(int64 Val, CodeLineInfo& Line)
{
    AConstExpr* Ret = AstPool::New<AConstExpr>(Line);
    Ret->Type = IT_int;
    Ret->IntVal = Val;
    return Ret;
}

AConstExpr* AConstExpr::NewFlt(double Val, CodeLineInfo& Line)
{
    AConstExpr* Ret = AstPool::New<AConstExpr>(Line);
    Ret->Type = IT_float;
    Ret->FltVal = Val;
    return Ret;
}

AConstExpr* AConstExpr::NewBool(bool Val, CodeLineInfo& Line)
{
    AConstExpr* Ret = AstPool::New<AConstExpr>(Line);
    Ret->Type = IT_bool;
    Ret->BoolVal = Val;
    return Ret;
}

AConstExpr* AConstExpr::NewStr(const TCHAR* Val, CodeLineInfo& Line)
{
    AConstExpr* Ret = AstPool::New<AConstExpr>(Line);
    Ret->Type = IT_string;
    Ret->StrVal = Val;
    return Ret;
}

AConstExpr* AConstExpr::NewNil(CodeLineInfo& Line)
{
    AConstExpr* Ret = AstPool::New<AConstExpr>(Line);
    Ret->Type = IT_nil;
    return Ret;
}


EVisitStatus AConstExpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus AConstExpr::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

}

