#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{



class AConstExpr : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    AConstExpr();
    virtual ~AConstExpr();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    virtual bool IsConstant();

    static AConstExpr* NewInt(int64 Val, CodeLineInfo& Line);
    static AConstExpr* NewFlt(double Val, CodeLineInfo& Line);
    static AConstExpr* NewBool(bool Val, CodeLineInfo& Line);
    static AConstExpr* NewStr(const TCHAR* Val, CodeLineInfo& Line);
    static AConstExpr* NewNil(CodeLineInfo& Line);
    EIntrinsicType Type;
    int64      IntVal;
    double     FltVal;
    bool       BoolVal;
    OLString   StrVal;

};

}
