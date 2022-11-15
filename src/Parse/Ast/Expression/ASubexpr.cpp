#include "ASubexpr.h"

namespace OL
{
STRUCT_RTTI_BEGIN(Operand)
    RTTI_MEMBER(Exp)
    RTTI_MEMBER(Op)
STRUCT_RTTI_END(Operand)

RTTI_BEGIN_INHERITED(ASubexpr, AExpr)
    RTTI_MEMBER(FirstUniOp)
    RTTI_STRUCT_MEMBER(OperandList, Operand)
    RTTI_MEMBER(IsLeftAffinity)
RTTI_END(ASubexpr)



ASubexpr::ASubexpr()
    : AExpr(), FirstUniOp(UO_None), IsLeftAffinity(true)
{
}

ASubexpr::~ASubexpr()
{
}

EVisitStatus ASubexpr::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}



EVisitStatus ASubexpr::Accept(RecursiveVisitor* Vis)
{
    EVisitStatus Status = Vis->BeginVisit(SThis);
    RETURN_BY_STATUS(Status);

    for(int i = 0; i < OperandList.Count(); i++)
    {
        Status = OperandList[i].Exp->Accept(Vis);
        RETURN_BY_STATUS(Status);
    }

    Status = Vis->EndVisit(SThis);
    RETURN_BY_STATUS(Status);

    return VS_Continue;
}

bool ASubexpr::IsConstant()
{
    for(int i = 0; i < OperandList.Count(); i++)
    {
        if(OperandList[i].Exp->IsConstant() == false)
            return false;
    }
    return true;
}

EUniOp ASubexpr::ConvertUniOp(ETokenType Tk)
{
    switch (Tk)
    {
        case TKS_sub:
            return UO_Minus;
        case TKS_sharp:
            return UO_Length;
        case TKS_bnot:
            return UO_BNot;
        case TKK_not:
            return UO_Not;
    }

    return UO_None;
}

EBinOp ASubexpr::ConvertBinOp(ETokenType Tk)
{
    switch (Tk)
    {
        case TKS_add:   return BO_Add;
        case TKS_sub:   return BO_Sub;
        case TKS_mul:  return BO_Mul;
        case TKS_div:   return BO_Div;
        case TKS_idiv:   return BO_IDiv;
        case TKS_dot2:   return BO_Cat;
        case TKK_and:   return BO_And;
        case TKK_or:   return BO_Or;
        case TKS_lshift:   return BO_ShiftL;
        case TKS_rshift:   return BO_ShiftR;
        case TKS_g:   return BO_Greater;
        case TKS_l:   return BO_Less;
        case TKS_ge:   return BO_GreaterEqual;
        case TKS_le:   return BO_LessEqual;
        case TKS_power:   return BO_LessEqual;
        case TKS_ne:       return BO_NotEqual;
        case TKS_eq:       return BO_Equal;
    }
    return BO_None;
}



// Do not change the order, must match the enum definition
BinOpInfo GBinOpData[] = 
{
    {BO_Add, 10, true, "+"},
    {BO_Sub, 10, true, "-"},
    {BO_Mul, 11, true, "*"},
    {BO_Div, 11, true, "/"},
    {BO_IDiv, 11, true, "//"},
    {BO_Cat, 9, false, ".."},
    {BO_And, 2, true, "and"},
    {BO_Or, 1, true, "or"},
    {BO_ShiftL, 7, true, "<<"},
    {BO_ShiftR, 7, true, ">>"},
    {BO_Greater, 3, true, ">"},
    {BO_Less, 3, true, "<"},
    {BO_GreaterEqual, 3, true, ">="},
    {BO_LessEqual, 3, true, "<="},
    {BO_Power, 14, false, "^"},
    {BO_NotEqual, 3, true, "~="},
    {BO_Equal, 3, true, "=="},
    { BO_None, 0, false, ""}
};


BinOpInfo& ASubexpr::GetBinOpInfo(EBinOp Op)
{
    OL_ASSERT(Op < (int)BO_None);

    return GBinOpData[(int)Op];
}


void ASubexpr::AddOperand(EBinOp Op, AExpr* Exp)
{
    Operand NewOperand;
    NewOperand.Op = Op;
    NewOperand.Exp = SPtr<AExpr>(Exp);
    
    OperandList.Add(NewOperand);


}

int ASubexpr::GetBinOpPrio()
{
    if(OperandList.Count() <= 1)
        return -1;
    
    return GetBinOpInfo(OperandList[1].Op).Priority;
}

const TCHAR* ASubexpr::UniopToString(EUniOp Op)
{
    if(Op == UO_Minus)      return "-";
    if(Op == UO_Length)      return "#";
    if(Op == UO_Not)      return "not";
    if(Op == UO_BNot)      return "~";

    return nullptr;

}
const TCHAR* ASubexpr::BinopToString(EBinOp Op)
{
    return GetBinOpInfo(Op).Desc;
}
}

