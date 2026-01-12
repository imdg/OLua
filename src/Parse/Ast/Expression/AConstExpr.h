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

    static AConstExpr* NewInt(int64 Val, SourceRange& Line);
    static AConstExpr* NewFlt(double Val, SourceRange& Line);
    static AConstExpr* NewBool(bool Val, SourceRange& Line);
    static AConstExpr* NewStr(const TCHAR* Val, SourceRange& Line);
    static AConstExpr* NewNil(SourceRange& Line);
    EIntrinsicType Type;
    int64      IntVal;
    double     FltVal;
    bool       BoolVal;
    OLString   StrVal;

};

}
