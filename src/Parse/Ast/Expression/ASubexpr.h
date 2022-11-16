#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AExpr.h"
#include "Lexer.h"

namespace OL
{

RTTI_ENUM(EBinOp,
    BO_Add,
    BO_Sub,
    BO_Mul,
    BO_Div,
    BO_IDiv,
    BO_Cat,
    BO_And,
    BO_Or,
    BO_ShiftL,
    BO_ShiftR,
    BO_Greater,
    BO_Less,
    BO_GreaterEqual,
    BO_LessEqual,
    BO_Power,
    BO_NotEqual,
    BO_Equal,
    BO_None
);

RTTI_ENUM(EUniOp,
    UO_Minus,
    UO_Length,
    UO_Not,
    UO_BNot,
    UO_None
);

struct BinOpInfo
{
    EBinOp Op;
    int Priority;
    bool IsLeftAffinity;
    const TCHAR* Desc;
};


class ASubexpr;
struct Operand
{
    SPtr<AExpr> Exp;
    EBinOp         Op;
};

class ASubexpr : public AExpr
{
    DECLEAR_RTTI()
public:
    ASubexpr();
    virtual ~ASubexpr();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    virtual bool IsConstant();
    void AddOperand(EBinOp Op, AExpr* Exp);
    int GetBinOpPrio();

    static EUniOp ConvertUniOp(ETokenType Tk);
    static EBinOp ConvertBinOp(ETokenType Tk);
    static BinOpInfo& GetBinOpInfo(EBinOp Op);

    static const TCHAR* UniopToString(EUniOp Op);
    static const TCHAR* BinopToString(EBinOp Op);

    EUniOp FirstUniOp;
    OLList< Operand > OperandList;
    bool IsLeftAffinity;
};

}
