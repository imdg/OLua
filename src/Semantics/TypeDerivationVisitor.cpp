/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "TypeDerivationVisitor.h"
#include "CompileMsg.h"
#include "AstInclude.h"
#include "TypeDescBase.h"
#include "MapType.h"
#include "ClassType.h"
#include "ArrayType.h"
#include "InterfaceType.h"
#include "IntrinsicType.h"
#include "VoidHolder.h"
#include "ConstructorType.h"
#include "EnumType.h"
#include "TupleType.h"
#include "Logger.h"
#include "VariableParamHolder.h"
#include "BuiltinLib.h"

#define ASSERT_CHILD_NUM(index, count) OL_ASSERT((count) == (DeriStack.Count() - 1 - (index)))

namespace OL
{

TypeDeriContex::TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType,  bool InIsConst )
    : Node(InNode), PrimaryType(InPrimaryType), IsType(false), IsConst(InIsConst), IsVariableParamRef(false), IsNilable(false)
{

}

TypeDeriContex::TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType , bool InIsConst, bool InIsType )
    : Node(InNode), PrimaryType(InPrimaryType), IsType(InIsType), IsConst(InIsConst), IsVariableParamRef(false), IsNilable(false)
{

}

TypeDerivationVisitor::TypeDerivationVisitor(SymbolTable& InSymboTable, CompileMsg& InCM, BuildSetting& InBuildSetting )
    : ScopeVisitorBase(InSymboTable), CM(InCM), Setting(InBuildSetting)
{

}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ASubexpr> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ASubexpr> Node)
{
    int Index = IndexStack.PickPop();

    ASSERT_CHILD_NUM(Index, Node->OperandList.Count());

    SPtr<TypeDescBase> CurrType = nullptr;
    bool ResultIsNilable = false;
    if(Node->FirstUniOp != UO_None)
    {
        CurrType = DeriStack[Index + 1].PrimaryType->AcceptUniOp(Node->FirstUniOp);
        if(CurrType == nullptr)
        {
            CM.Log(CMT_UniopNotMatch, Node->Line, ASubexpr::UniopToString(Node->FirstUniOp)
                , DeriStack[Index + 1].PrimaryType->ToString(DeriStack[Index + 1].IsNilable).CStr());
            return VS_Stop;
        }
    }
    else
    {
        CurrType = DeriStack[Index + 1].PrimaryType;
        ResultIsNilable = DeriStack[Index + 1].IsNilable;
    }

    

    for(int i = 1; i < Node->OperandList.Count(); i++)
    {
        SPtr<TypeDescBase> Target = DeriStack[Index + 1 + i].PrimaryType;
        SPtr<ABase> TargetNode = DeriStack[Index + 1 + i].Node;
        bool TargetIsNilable = DeriStack[Index + 1 + i].IsNilable;
        // BO_NilCoalesc is handled here, rather than CurrType->AcceptBinOp 
        
        if(Node->OperandList[i].Op == BO_NilCoalesc)
        {
            if(ResultIsNilable == false)
            {
                CM.Log(CMT_NonnilableCoalescing, Node->Line);
            }
            else if(TargetIsNilable == true)
            {
                CM.Log(CMT_NilableCoalescingNilable, Node->Line);
            }

            if(MatchType(Target, false, CurrType, false, TargetNode, false) == TCR_NoWay)
            {
                return VS_Stop;
            }
            ResultIsNilable = false;
            // CurrType does not change
        }
        else
        {
            OperatorResult Result = CurrType->AcceptBinOp(Node->OperandList[i].Op, Target, TargetIsNilable);
            if(Result.IsNilable)
                ResultIsNilable = true;
            if(Result.Type == nullptr)
            {
                CM.Log(CMT_BinopNotMatch, Node->Line, ASubexpr::BinopToString(Node->OperandList[i].Op), CurrType->ToString(ResultIsNilable).CStr(), Target->ToString(TargetIsNilable).CStr());
                return VS_Stop;
            }
            CurrType = Result.Type;
        }

        
    }
    
    if(CurrType == nullptr)
    {

    }

    DeriStack[Index].PrimaryType = CurrType;
    DeriStack[Index].IsNilable = ResultIsNilable;
    DeriStack.PopTo(Index);

    Node->ExprType = CurrType;
    Node->IsNilable = ResultIsNilable;
    return VS_Continue;
}




EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ATypeCast> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ATypeCast> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    SPtr<TypeDescBase> TargetType = CurrScope->FindTypeByNode(Node->TargetType.Get(), true);

    TypeDeriContex& Top = DeriStack.Top();
    // TypeCast does not change nilable state, so use 'true' to do only type validation itself
    // Result keeps its original nilable state
    MatchType(Top.PrimaryType, true, TargetType, true, Top.Node, true);
    bool IsConst = Top.IsConst;
    bool IsNilable = Top.IsNilable;
    DeriStack.Pop();


    DeriStack.Top().PrimaryType = TargetType;
    DeriStack.Top().IsConst = IsConst;
    DeriStack.Top().IsNilable = IsNilable;

    Node->ExprType = TargetType;
    Node->IsNilable = IsNilable;
    return VS_Continue;
}



EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ADotMember> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ADotMember> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    bool OK = true;
    TypeDeriContex DotTarget = DeriStack.PickPop();

    SPtr<TypeDescBase> MemberType = nullptr;
    bool IsConst = false;
    bool IsNilable = DotTarget.IsNilable;

    if(IsNilable && Setting.NilSafety != VL_None)
    {
        CM.Log(CMT_DerefNilable, Node->Line);
    }

    if(DotTarget.PrimaryType->ActuallyIs<MapType>())
    {
        MemberType = DerefMap(DotTarget.PrimaryType->ActuallyAs<MapType>(), Node->Field, Node.Get());
        IsConst = DotTarget.IsConst;
        if(DotTarget.PrimaryType->ActuallyAs<MapType>()->IsValueNilable)
            IsNilable = true;
    }
    else if(DotTarget.PrimaryType->ActuallyIs<ClassType>())
    {
        DerefResult Deref = DerefClass(DotTarget.PrimaryType->ActuallyAs<ClassType>(), Node->Field, Node.Get(), DotTarget.IsType, false);
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;
    }
    else if(DotTarget.PrimaryType->ActuallyIs<InterfaceType>())
    {
        DerefResult Deref =  DerefInterface(DotTarget.PrimaryType->ActuallyAs<InterfaceType>(), Node->Field, Node.Get());
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;
    }
    else if(DotTarget.PrimaryType->ActuallyIs<EnumType>() && DotTarget.IsType == true)
    {
        DerefResult Deref = DerefEnum(DotTarget.PrimaryType->ActuallyAs<EnumType>(), Node->Field, Node.Get(), true);
        if(Deref.Type != nullptr)
        {
            MemberType = Deref.Type;
        }
    }


    if(MemberType != nullptr)
    {
        DeriStack.Top().PrimaryType = MemberType;
        DeriStack.Top().IsConst = DotTarget.IsConst ? true : IsConst;
        DeriStack.Top().IsNilable = IsNilable;
        Node->ExprType = MemberType;
        Node->IsNilable = IsNilable;
    }
    else
    {
        if(Setting.AllowImplicitAny)
        {    
            DeriStack.Top().PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
            DeriStack.Top().IsNilable = true;
            Node->ExprType = DeriStack.Top().PrimaryType;
            Node->IsNilable = true;
        }
        else
            return VS_Stop;
    }

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ABracketMember> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ABracketMember> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 2);
    
    TypeDeriContex Field = DeriStack.PickPop();
    TypeDeriContex Target = DeriStack.PickPop();

    if(Target.IsNilable && Setting.NilSafety != VL_None)
    {
        CM.Log(CMT_DerefNilable, Node->Line);
    }

    if(Target.PrimaryType->ActuallyIs<ArrayType>())
    {
        if(Field.IsNilable && Setting.NilSafety != VL_None)
        {
            CM.Log(CMT_NilableAsIndex, Node->Line);
        }
        if(MatchType(Field.PrimaryType, false, IntrinsicType::CreateFromRaw(IT_int), false, Field.Node, false) == TCR_NoWay)
        {
            CM.Log(CMT_ArrayNeedIntIndex, Node->Line);
        }
        DeriStack.Top().PrimaryType = Target.PrimaryType->ActuallyAs<ArrayType>()->ElemType.Lock();
        DeriStack.Top().IsConst = Target.IsConst;

        bool IsNilable = false;
        if(Target.IsNilable || Target.PrimaryType->ActuallyAs<ArrayType>()->IsElemNilable)
            IsNilable = true;

        DeriStack.Top().IsNilable = IsNilable;
        Node->ExprType = DeriStack.Top().PrimaryType;
        Node->IsNilable = IsNilable;
    }
    else if(Target.PrimaryType->ActuallyIs<MapType>())
    {
        if(Field.IsNilable && Setting.NilSafety != VL_None)
        {
            CM.Log(CMT_NilableAsIndex, Node->Line);
        }
        if(MatchType(Field.PrimaryType, false, Target.PrimaryType->ActuallyAs<MapType>()->KeyType.Lock(), false, Field.Node, false) == TCR_NoWay)
        {
            CM.Log(CMT_MapKeyTypeMismatch, Node->Line);
        }

        DeriStack.Top().PrimaryType = Target.PrimaryType->ActuallyAs<MapType>()->ValueType.Lock();
        DeriStack.Top().IsConst = Target.IsConst;
        
        bool IsNilable = false;
        if(Target.IsNilable || Target.PrimaryType->ActuallyAs<MapType>()->IsValueNilable)
            IsNilable = true;

        DeriStack.Top().IsNilable = IsNilable;
        Node->ExprType = DeriStack.Top().PrimaryType;
        Node->IsNilable = IsNilable;
    }
    else
    {
        CM.Log(CMT_InvalidBracket, Node->Line, Target.PrimaryType->ToString(Target.IsNilable).CStr());
        return VS_Stop;
    }

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ANormalCall> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ANormalCall> Node)
{
    int Index = IndexStack.PickPop();
    //ASSERT_CHILD_NUM(Index, Node->)

    SPtr<TypeDescBase> FuncBase = DeriStack[Index + 1].PrimaryType;
    bool IsFuncNilable = DeriStack[Index + 1].IsNilable;
    if(IsFuncNilable)
    {
        CM.Log(CMT_CallNilable, Node->Line);
    }
    if(FuncBase->ActuallyIs<FuncSigniture>() == false)
    {
        if(FuncBase->IsAny() == false)
        {
            CM.Log(CMT_RequirFunc, Node->Line);
            return VS_Stop;
        }
        else
        {
            // Any type, Do not validate
            DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
            DeriStack[Index].IsNilable = true;
            DeriStack.PopTo(Index);
            Node->ExprType = DeriStack[Index].PrimaryType;
            Node->IsNilable = true;

            return VS_Continue;
        }
    }

    SPtr<FuncSigniture> Func = FuncBase->ActuallyAs<FuncSigniture>();
    bool Match = MatchFuncCall(Func, Index, Node);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AColonCall> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AColonCall> Node)
{
    int Index = IndexStack.PickPop();
    // Assert
    
    SPtr<TypeDescBase> FuncOwner = DeriStack[Index + 1].PrimaryType;
    bool IsOwnerConst = DeriStack[Index + 1].IsConst;
    bool IsType = DeriStack[Index + 1].IsType;

    SPtr<TypeDescBase> MemberType = nullptr;
    bool IsConst = false;
    bool IsNilable = DeriStack[Index + 1].IsNilable;

    if(IsNilable && Setting.NilSafety != VL_None)
    {
        CM.Log(CMT_DerefNilable, Node->Line);
    }

    if(FuncOwner->ActuallyIs<MapType>())
    {
        MemberType = DerefMap(FuncOwner->ActuallyAs<MapType>(), Node->NameAfter, Node.Get());
        if(FuncOwner->ActuallyAs<MapType>()->IsValueNilable)
            IsNilable = true;
    }
    else if(FuncOwner->ActuallyIs<ClassType>())
    {
        DerefResult Deref =  DerefClass(FuncOwner->ActuallyAs<ClassType>(), Node->NameAfter, Node.Get(), DeriStack[Index + 1].IsType, true);
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;

    }
    else if(FuncOwner->ActuallyIs<InterfaceType>())
    {
        DerefResult Deref =  DerefInterface(FuncOwner->ActuallyAs<InterfaceType>(), Node->NameAfter, Node.Get());
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;
    }
    else if(FuncOwner->IsAny())
    {
        // Any type owner, Do not validate
        DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
        DeriStack[Index].IsNilable = true;

        Node->ExprType = DeriStack[Index].PrimaryType ;
        Node->IsNilable = true;
        DeriStack.PopTo(Index);
        return VS_Continue;
    }
    else if(FuncOwner->ActuallyIs<IntrinsicType>() && IsType == false)
    {
        DerefResult Deref = DerefIntrinsic(FuncOwner->ActuallyAs<IntrinsicType>(), Node->NameAfter, Node.Get());
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;
    }
    else if(FuncOwner->ActuallyIs<EnumType>() && IsType == false)
    {
        DerefResult Deref = DerefEnum(FuncOwner->ActuallyAs<EnumType>(), Node->NameAfter, Node.Get(), false);
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
        if(Deref.IsNilable)
            IsNilable = true;
    }

    if(MemberType == nullptr)
    {
        ERROR(LogCompile, T("Internal error while dereferencing member %s (%d,%d)"), Node->NameAfter.CStr(), Node->Line.Line, Node->Line.Col);
        DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
        DeriStack[Index].IsNilable = true;
        Node->ExprType = DeriStack[Index].PrimaryType ;
        Node->IsNilable = true;

        DeriStack.PopTo(Index);
        return VS_Continue;

    }

    if(MemberType->ActuallyIs<FuncSigniture>() == false)
    {
        if(MemberType->IsAny())
        {
            // Any type, Do not validate
            DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
            DeriStack[Index].IsNilable = true;

            Node->ExprType = DeriStack[Index].PrimaryType ;
            Node->IsNilable = true;
            Node->ColonMemberType = IntrinsicType::CreateFromRaw(IT_any);
            DeriStack.PopTo(Index);
            return VS_Continue;
        }
        else
        {
            CM.Log(CMT_RequirFunc, Node->Line);
            return VS_Stop;
        }
    }
    else
    {
        Node->ColonMemberType = MemberType;
    }

    if(!IsConst && IsOwnerConst)
    {
        CM.Log(CMT_ConstRestrictOwner, Node->Line);
        return VS_Stop;
    }



    bool Match = MatchFuncCall(MemberType->ActuallyAs<FuncSigniture>(), Index, Node);

    return VS_Continue;

}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AFuncExpr> Node)
{
    SPtr<TypeDescBase> FuncBase = CurrScope->FindTypeByNode(Node.Get(), false);
    OL_ASSERT(FuncBase->ActuallyIs<FuncSigniture>());

    DeriStack.Add(TypeDeriContex(Node, FuncBase, false));
    DeriStack.Top().IsNilable = false;

    IndexStack.Add(DeriStack.Count() - 1);
    Node->ExprType = FuncBase;
    Node->IsNilable = false;
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AFuncExpr> Node)
{
    IndexStack.Pop();
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AConstructor> Node)
{
    SPtr<TypeDescBase> CtorType = CurrScope->FindTypeByNode(Node.Get(), false);
    DeriStack.Add(TypeDeriContex(Node, CtorType, false));
    DeriStack.Top().IsNilable = false;

    Node->ExprType = CtorType;
    Node->IsNilable = false;
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AConstructor> Node)
{
    int Index = IndexStack.PickPop();
    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<AConstExpr> Node)
{
    SPtr<TypeDescBase> Const = IntrinsicType::CreateFromRaw(Node->Type);
    DeriStack.Add(TypeDeriContex(Node, Const, true));
    DeriStack.Top().IsNilable = false;
    Node->ExprType = Const;
    Node->IsNilable = false;
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<AVarRef> Node)
{
    SPtr<Reference> Ref = CurrScope->FindRefByNode(Node.Get());
    OL_ASSERT(Ref != nullptr);

    if(Ref->Decl == nullptr)
    {
        DeriStack.Add(TypeDeriContex(Node, IntrinsicType::CreateFromRaw(IT_any), false));
        DeriStack.Top().IsNilable = true;
        CM.Log(CMT_ImplicitGlobal, Node->Line, Node->VarName.CStr());
        Node->ExprType = IntrinsicType::CreateFromRaw(IT_any, true);
        Node->IsNilable = true;
    }
    else
    {
        
        if(SymbolScope::IsResolvedType(Ref->Type))
        {
            SPtr<TypeDescBase> TypeDesc = Ref->DeclScope->FindTypeByNode(Ref->Decl->DeclNode.Get(), false);
            DeriStack.Add(TypeDeriContex(Node,TypeDesc, false, true));
            DeriStack.Top().IsNilable = false; // Underneath a type name it is the static scope of a type, so it is always non-nilable 
        }
        else
        {
            // Check if this is in a ctor and accessing class member var.
            // In this case const member can be treated as non-const
            bool IsConst = Ref->Decl->IsConst;
            bool IsNilable = Ref->Decl->IsNilable;
            SPtr<FuncSigniture> OwnerFunc = GetOuterFunc();
            if(OwnerFunc != nullptr && OwnerFunc->DeclNode->Is<AClassContructor>() == true)
            {
                SPtr<ClassType> Class = GetOuterClass();
                if(Class != nullptr && Class->IsMethod(OwnerFunc))
                {
                    FindMemberResult Found = Class->FindMemberByDeclNode(Ref->Decl->DeclNode.Lock(), true);
                    if(Found.FromClass == nullptr || Found.FromInterface != nullptr)
                    {
                        IsConst = false;
                    }
                }
            }
            DeriStack.Add(TypeDeriContex(Node, Ref->Decl->ValueTypeDesc.Lock(),  IsConst,  false));
            DeriStack.Top().IsNilable = IsNilable;
        }
    }
    Node->ExprType = DeriStack.Top().PrimaryType;
    Node->IsNilable = DeriStack.Top().IsNilable;
    

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<ASuper> Node)
{
    SPtr<ClassType> Class = GetOuterClass();
    if(Class == nullptr)
    {
        CM.Log(CMT_IllegalSuper, Node->Line);
        return VS_Stop;
    }
    SPtr<ClassType> SuperClass = Class->GetSuperClass();
    if(Class == nullptr)
    {
        CM.Log(CMT_IllegalSuper, Node->Line);
        return VS_Stop;
    }

    SPtr<FuncSigniture> Func = GetOuterFunc();
    if(Func == nullptr)
    {
        CM.Log(CMT_IllegalSuper, Node->Line);
        return VS_Stop;
    }
    

    // Check current function is a method of outer class
    // 'super' is only valid in a member function of a class with a base class
    if(Class->IsMethod(Func) == false)
    {
        CM.Log(CMT_IllegalSuper, Node->Line);
        return VS_Stop;
    }

    bool IsConst = false;
    FindMemberResult Found = Class->FindMemberByDeclNode(Func->DeclNode.Lock(), false);
    if(Found.FromClass != nullptr &&  Found.FromClass->Flags & CMF_Const)
    {
        IsConst = true;
    }


    DeriStack.Add(TypeDeriContex(Node, SuperClass, IsConst));
    DeriStack.Top().IsNilable = false;
    Node->ExprType = SuperClass;
    Node->IsNilable = false;

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<ASelf> Node)
{
    SPtr<ClassType> Class = GetOuterClass();
    if(Class == nullptr)
    {
        CM.Log(CMT_IllegalSelf, Node->Line);
        return VS_Stop;
    }

    SPtr<FuncSigniture> Func = GetOuterFunc();
    if(Func == nullptr)
    {
        CM.Log(CMT_IllegalSelf, Node->Line);
        return VS_Stop;
    }

    // Check current function is a method of outer class
    // 'super' is only valid in a member function of a class 
    if(Class->IsMethod(Func) == false)
    {
        CM.Log(CMT_IllegalSelf, Node->Line);
        return VS_Stop;
    }

        bool IsConst = false;
    FindMemberResult Found = Class->FindMemberByDeclNode(Func->DeclNode.Lock(), false);
    if(Found.FromClass != nullptr &&  Found.FromClass->Flags & CMF_Const)
    {
        IsConst = true;
    }

    DeriStack.Add(TypeDeriContex(Node, Class, IsConst));
    DeriStack.Top().IsNilable = false;
    Node->ExprType = Class;
    Node->IsNilable = false;
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AParentheses> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AParentheses> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    DeriStack[Index].PrimaryType = DeriStack.Top().PrimaryType;
    DeriStack[Index].IsConst = DeriStack.Top().IsConst;
    DeriStack[Index].IsNilable = DeriStack.Top().IsNilable;

    Node->ExprType = DeriStack.Top().PrimaryType;
    Node->IsNilable = DeriStack.Top().IsNilable;

    DeriStack.Pop();
    return VS_Continue;
}


EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ANilableUnwrap> Node)
{
    DeriStack.Add(TypeDeriContex(Node, nullptr, false));
    IndexStack.Add(DeriStack.Count() - 1);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ANilableUnwrap> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    DeriStack[Index].PrimaryType = DeriStack.Top().PrimaryType;
    DeriStack[Index].IsConst = DeriStack.Top().IsConst;
    if(DeriStack.Top().IsNilable == false && Setting.NilSafety != VL_None)
    {
        CM.Log(CMT_UnwrapNonnilable, Node->Line);
    }

    DeriStack[Index].IsNilable = false;

    Node->ExprType = DeriStack.Top().PrimaryType;
    Node->IsNilable = DeriStack.Top().IsNilable;

    DeriStack.Pop();
    return VS_Continue;
}


//---------------------------------------------------------------
//          Statements
//-------------------------------------------------------------------
EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ACallStat> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ACallStat> Node)
{
    int Index = IndexStack.PickPop();
    DeriStack.PopTo(Index);

    return VS_Continue;
}


EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AClassVar> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AClassVar> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Inits.Count());

    for(int i = 0; i < Node->Inits.Count(); i++)
    {
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
        SPtr<TypeDescBase> InitValType = DeriStack[Index + 1 + i].PrimaryType;
        SPtr<ABase> InitValueNode = DeriStack[Index + 1 + i].Node;
        bool InitValIsNilable = DeriStack[Index + 1 + i].IsNilable;
        MatchType(InitValType, InitValIsNilable, Decl->ValueTypeDesc.Lock(), Decl->IsNilable, InitValueNode, false );
        
    }

    // Validate non-nilable member without init expr
    if(Node->Inits.Count() < Node->Decls.Count())
    {
        for(int i = Node->Inits.Count(); i < Node->Decls.Count(); i++)
        {
            SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
            if(Decl->IsNilable == false && Setting.NilSafety != VL_None)
            {
                CM.Log(CMT_NonnilableMember, Node->Line, Node->Decls[i]->VarName.CStr());
            }
        }
    }
    DeriStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AEnumItem> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AEnumItem> Node)
{
    int Index = IndexStack.PickPop();
    if(Node->ValueExpr != nullptr)
    {
        ASSERT_CHILD_NUM(Index, 1);
        TypeDeriContex InitExpr = DeriStack[Index + 1];
        MatchType(InitExpr.PrimaryType, InitExpr.IsNilable, IntrinsicType::CreateFromRaw(IT_int), false, InitExpr.Node, false);
    }
    DeriStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AAssignment> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AAssignment> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->LeftExprs.Count() * 2);

    for(int i = 0; i < Node->LeftExprs.Count(); i++)
    {
        TypeDeriContex LVal = DeriStack[Index + 1 + i];
        TypeDeriContex RVal = DeriStack[Index + Node->LeftExprs.Count() + 1 + i];

        if(LVal.IsConst == true)
        {
            CM.Log(CMT_ConstRestrictAssign, Node->Line);
        }
        
        MatchType(RVal.PrimaryType, RVal.IsNilable, LVal.PrimaryType, LVal.IsNilable, RVal.Node, false);
    }
    DeriStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ADoWhile> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ADoWhile> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);
    TypeDeriContex Cond = DeriStack[Index + 1];

    MatchType(Cond.PrimaryType, Cond.IsNilable, IntrinsicType::CreateFromRaw(IT_bool), true, Cond.Node, false);
    DeriStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AForList> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AForList> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    SPtr<SymbolScope> InsideScope = CurrSymbolTable.FindByBlock(Node->MainBlock.Get());

    SPtr<TypeDescBase> IteratorType = DeriStack[Index + 1].PrimaryType;
    if(IteratorType->ActuallyIs<ArrayType>())
    {
        SPtr<ArrayType> Array = IteratorType->ActuallyAs<ArrayType>();
        if(Node->VarList.Count() == 1)
        {
            // One var decleared, see it as array element
            SPtr<Declearation> DeclInfoValue = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            MatchType(Array->ElemType.Lock(), Array->IsElemNilable,  DeclInfoValue->ValueTypeDesc.Lock(), DeclInfoValue->IsNilable, Node->Iterator, false, false);
        }
        else if(Node->VarList.Count() == 2)
        {
            // Two var decleared, see them as index and element
            SPtr<Declearation> DeclInfoIndex = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            SPtr<Declearation> DeclInfoValue = InsideScope->FindDeclByNode(Node->VarList[1].Get());
            MatchType(IntrinsicType::CreateFromRaw(IT_int), false, DeclInfoIndex->ValueTypeDesc.Lock(), DeclInfoIndex->IsNilable, Node->Iterator, false, false);
            MatchType(Array->ElemType.Lock(), Array->IsElemNilable,  DeclInfoValue->ValueTypeDesc.Lock(), DeclInfoValue->IsNilable, Node->Iterator, false, false);
        }
        else
        {
            CM.Log(CMT_ArrayIterMismatch, Node->Iterator->Line);
        }

    }
    else if(IteratorType->ActuallyIs<MapType>())
    {
        SPtr<MapType> Map = IteratorType->ActuallyAs<MapType>();
        if(Node->VarList.Count() != 2)
        {
            CM.Log(CMT_MapIterMismatch, Node->Iterator->Line);
        }
        else
        {
            SPtr<Declearation> KeyDeclInfo = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            // if(KeyDeclInfo->IsNilable && Setting.NilSafety != VL_None)
            // {
            //     CM.Log(CMT_KeyNilable, Node->Line);
            // }
            SPtr<Declearation> ValueDeclInfo = InsideScope->FindDeclByNode(Node->VarList[1].Get());

            MatchType(Map->KeyType.Lock(), false,  KeyDeclInfo->ValueTypeDesc.Lock(), false, Node->VarList[0], false, false);
            MatchType(Map->ValueType.Lock(), Map->IsValueNilable, ValueDeclInfo->ValueTypeDesc.Lock(), ValueDeclInfo->IsNilable, Node->VarList[1], false, false);
        }
    }
    else if(IteratorType->ActuallyIs<ConstructorType>())
    {
        
        SPtr<Declearation> DeclInfo2;
        SPtr<TypeDescBase> Type1;
        SPtr<TypeDescBase> Type2;
        if(Node->VarList.Count() != 1 || Node->VarList.Count() != 2)
        {
            CM.Log(CMT_IterMismatch, Node->Iterator->Line);
        }
        if(Node->VarList.Count() >= 1)
        {
            SPtr<Declearation> DeclInfo1 = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            Type1 = DeclInfo1->ValueTypeDesc.Lock();
        }
        if(Node->VarList.Count() >= 2)
        {
            SPtr<Declearation> DeclInfo2 = InsideScope->FindDeclByNode(Node->VarList[1].Get());
            Type2 = DeclInfo2->ValueTypeDesc.Lock();
        }

        ETypeValidation Result = IteratorType->ActuallyAs<ConstructorType>()->ValidateIterator(Type1, Type2);
        if(Result == TCR_NoWay)
        {
            CM.Log(CMT_IterMismatch, Node->Iterator->Line);
        }
    }
    else if(IteratorType->IsAny())
    {

    }
    else if(IteratorType->ActuallyIs<TupleType>())
    {
        SPtr<TupleType> Tuple = IteratorType->ActuallyAs<TupleType>();

        auto ValidateIteratorFun = [&]() ->bool
        {
            // if(Tuple->Subtypes.Count() != 3)
            // {
            //     // error
            //     CM.Log(CMT_IterTupleNum, Node->Line);
            //     return false;
            // }


            if(Tuple->Subtypes[0].Type->ActuallyIs<FuncSigniture>() == false)
            {
                // error
                CM.Log(CMT_IterEntryType, Node->Line);
                return false;
            }
            if(Tuple->Subtypes[0].IsNilable == true && Setting.NilSafety != VL_None)
            {
                CM.Log(CMT_NilableIter, Node->Line);
            }

            SPtr<FuncSigniture> IterFun = Tuple->Subtypes[0].Type.Lock()->ActuallyAs<FuncSigniture>();
            // if(IterFun->Params.Count() != 2)
            // {
            //     // error
            //     CM.Log(CMT_IterFuncType, Node->Line);
            //     return false;
            // }
            ETypeValidation Result = TCR_NoWay;
            if(IterFun->Params.Count() >= 1)
            {
                // First param must be compatible with the second in tuple
                SPtr<TypeDescBase> StableVarType = Tuple->Subtypes[1].Type.Lock();
                Result = MatchType(StableVarType, true, IterFun->Params[0].Type.Lock(), true, Tuple->DeclNode.Lock(), false, false);
                if(Result == TCR_NoWay)
                {
                    // error
                    CM.Log(CMT_IterTargetType, Node->Line);
                    return false;
                }
            }
            // Return value contains no less than Node->VarList
            if( IterFun->Returns.Count() <  Node->VarList.Count())
            {
                // error
                CM.Log(CMT_IterItemNum, Node->Line);
                return false;
            }
            
            if(IterFun->Params.Count() >= 2 && Tuple->Subtypes.Count() >= 3)
            {
                SPtr<TypeDescBase> IndexingVarType = Tuple->Subtypes[2].Type.Lock();
                // Second param must be compatible with indexing value(third in tuple)
                Result = MatchType(IndexingVarType, true, IterFun->Params[1].Type.Lock(), true, Tuple->DeclNode.Lock(), false, false);
                if(Result == TCR_NoWay)
                {
                    // error
                    CM.Log(CMT_IterIndexType, Node->Line);
                    return false;
                }

                // first return value must be compatible with indexing type
                Result = MatchType(IndexingVarType, true, IterFun->Returns[0].Type.Lock(), true, Tuple->DeclNode.Lock(), false, false);
                if(Result == TCR_NoWay)
                {
                    // error
                    CM.Log(CMT_IterFuncIndexType, Node->Line);
                    return false;
                }
            }
            for(int i = 0; i < Node->VarList.Count(); i++)
            {
                SPtr<Declearation> DeclInfo = InsideScope->FindDeclByNode(Node->VarList[i].Get());
                Result = MatchType(IterFun->Returns[i].Type.Lock(), true, DeclInfo->ValueTypeDesc.Lock(), true, Tuple->DeclNode.Lock(), false, false);
                if(Result == TCR_NoWay)
                {
                    //error
                    CM.Log(CMT_IterValueType, Node->Line, i);
                    return false;
                }
            }

            return true;
        };

        ValidateIteratorFun();

    }
    else
    {
        CM.Log(CMT_NotAIterator, Node->Iterator->Line);
    }

    DeriStack.PopTo(Index);
    
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AForNum> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AForNum> Node)
{
    int Index = IndexStack.PickPop();
    int ExpectNum = 2;
    if(Node->Step != nullptr)
        ExpectNum = 3;

    ASSERT_CHILD_NUM(Index, ExpectNum);
    SPtr<IntrinsicType> IntType = IntrinsicType::CreateFromRaw(IT_int);
    MatchType(DeriStack[Index + 1].PrimaryType, DeriStack[Index + 1].IsNilable, IntType, false, DeriStack[Index + 1].Node, false);
    MatchType(DeriStack[Index + 2].PrimaryType, DeriStack[Index + 2].IsNilable, IntType, false, DeriStack[Index + 2].Node, false);
    if(ExpectNum == 3)
    {
        MatchType( DeriStack[Index + 3].PrimaryType, DeriStack[Index + 3].IsNilable, IntType, false, DeriStack[Index + 3].Node, false);
    }

    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AFuncDef> Node)
{
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AFuncDef> Node)
{
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AGlobal> Node)
{
    IndexStack.Add(DeriStack.Count() -1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AGlobal> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Inits.Count());

    OLList<TypeDeriContex> StackTypes;
    ExpandStackTuple(Index, StackTypes);

    for(int i = 0; i < StackTypes.Count(); i++)
    {
        if(i >= Node->Decls.Count())
            break;
        //SPtr<TypeDescBase> ExprType = StackTypes[i].PrimaryType; // DeriStack[Index + 1 + i].PrimaryType;
        //bool IsExprNilable = StackTypes[i].IsNilable;
        TypeDeriContex InitExpr =  StackTypes[i];
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
        if(Decl->ValueTypeDesc->IsImplicitAny() && InitExpr.PrimaryType->ActuallyIs<VariableParamHolder>() == false)
        {
            Decl->ValueTypeDesc = InitExpr.PrimaryType->DeduceLValueType(CurrScope);
            Decl->IsNilable = InitExpr.IsNilable;
        }
        else
            MatchType(InitExpr.PrimaryType, InitExpr.IsNilable, Decl->ValueTypeDesc.Lock(), Decl->IsNilable, InitExpr.Node, false);
    }

    // Non-nilables without initialization
    if(Node->Decls.Count() > StackTypes.Count())
    {
        for(int i = StackTypes.Count(); i < Node->Decls.Count(); i++)
        {
            SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
            if(Decl->IsNilable == false && Setting.NilSafety != VL_None)
            {
                CM.Log(CMT_NonnilableNotInit, Node->Line, Decl->Name.CStr() );
            }
        }
    }
    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ALocal> Node)
{
    IndexStack.Add(DeriStack.Count() -1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ALocal> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Inits.Count());

    OLList<TypeDeriContex> StackTypes;
    ExpandStackTuple(Index, StackTypes);

    for(int i = 0; i < StackTypes.Count(); i++)
    {
        if(i >= Node->Decls.Count())
            break;

        //SPtr<TypeDescBase> ExprType = StackTypes[i].PrimaryType; // DeriStack[Index + 1 + i].PrimaryType;
        //bool IsExprNilable = StackTypes[i].IsNilable;
        TypeDeriContex InitExpr =  StackTypes[i];

        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
        if(Decl->ValueTypeDesc->IsImplicitAny() && InitExpr.PrimaryType->ActuallyIs<VariableParamHolder>() == false)
        {
            Decl->ValueTypeDesc = InitExpr.PrimaryType->DeduceLValueType(CurrScope);
            Decl->IsNilable = InitExpr.IsNilable;
        }
        else
            MatchType(InitExpr.PrimaryType, InitExpr.IsNilable, Decl->ValueTypeDesc.Lock(), Decl->IsNilable, InitExpr.Node, false);
    }

    // Non-nilables without initialization
    if(Node->Decls.Count() > StackTypes.Count())
    {
        for(int i = StackTypes.Count(); i < Node->Decls.Count(); i++)
        {
            SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
            if(Decl->IsNilable == false && Setting.NilSafety != VL_None)
            {
                CM.Log(CMT_NonnilableNotInit, Node->Line, Decl->Name.CStr() );
            }
        }
    }

    DeriStack.PopTo(Index);
    return VS_Continue;
}


EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AIfStat> Node)
{
    IndexStack.Add(DeriStack.Count() -1);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AIfStat> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->ElseIfBlocks.Count() + 1);

    SPtr<IntrinsicType> BoolType = IntrinsicType::CreateFromRaw(IT_bool);
    TypeDeriContex MainCond = DeriStack[Index + 1];
    MatchType(MainCond.PrimaryType, MainCond.IsNilable, BoolType, true, MainCond.Node, false);

    for(int i = 0; i < Node->ElseIfBlocks.Count(); i++)
    {
        TypeDeriContex ElifCond = DeriStack[Index + 2 + i];
        MatchType( ElifCond.PrimaryType, ElifCond.IsNilable, BoolType, true, ElifCond.Node, false);
    }

    DeriStack.PopTo(Index);
    return VS_Continue;
}



EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<ARepeat> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<ARepeat> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    TypeDeriContex Cond = DeriStack[Index + 1];
    MatchType(Cond.PrimaryType, Cond.IsNilable, IntrinsicType::CreateFromRaw(IT_bool), true, Cond.Node, false );

    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AReturn> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AReturn> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Ret.Count());

    SPtr<FuncSigniture> Func = GetOuterFunc();
    if(Node->Ret.Count() != Func->Returns.Count())
    {
        CM.Log(CMT_ReturnMismatch, Node->Line, Func->Returns.Count(), Node->Ret.Count());
    }
    else
    {
        for(int i = 0; i < Node->Ret.Count(); i++)
        {
            SPtr<TypeDescBase> DeclType = Func->Returns[i].Type.Lock();
            bool DeclIsNilable = Func->Returns[i].IsNilable;

            TypeDeriContex StatVal = DeriStack[Index + 1 + i];
            MatchType(StatVal.PrimaryType, StatVal.IsNilable, DeclType, DeclIsNilable, StatVal.Node, false);
        }
    }
    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AWhile> Node)
{
    IndexStack.Add(DeriStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus TypeDerivationVisitor::EndVisit(SPtr<AWhile> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);
    TypeDeriContex Condi = DeriStack[Index + 1];

    MatchType(Condi.PrimaryType, Condi.IsNilable, IntrinsicType::CreateFromRaw(IT_bool), true, Condi.Node, false );
    DeriStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<AVariableParamRef> Node)
{
    SPtr<FuncSigniture> OuterFun = GetOuterFunc();
    if(OuterFun->Params.Count() > 0 && OuterFun->Params[OuterFun->Params.Count() - 1].IsVariableParam == true)
    {
        FuncParamDesc& ParamDesc = OuterFun->Params[OuterFun->Params.Count() - 1];
        DeriStack.Add(TypeDeriContex(Node, ParamDesc.Type.Lock(), (ParamDesc.Flags & FPF_Const) ? true : false ));
        DeriStack.Top().IsNilable = ParamDesc.IsNilable;
        Node->ExprType = ParamDesc.Type;
        Node->IsNilable = ParamDesc.IsNilable;
    }
    else
    {
        OL_ASSERT(0 && T("Invalid use of '...', should be checked in previous steps"));
    }
    return VS_Continue;
}

ETypeValidation TypeDerivationVisitor::MatchType(SPtr<TypeDescBase> From, bool IsFromNilable, SPtr<TypeDescBase> To, bool IsToNilable, SPtr<ABase> Node, bool IsExplicit, bool SaveConvertInfo )
{
    if(IsFromNilable && IsToNilable == false && Setting.NilSafety != VL_None)
    {
        CM.Log(CMT_NilableConvert, Node->Line);
    }

    if(IsToNilable == false && From->IsNil()) 
    {
        CM.Log(CMT_AssignNilToNonnilable, Node->Line);
    }

    ETypeValidation Result = From->ValidateConvert(To);
    switch (Result)
    {
    case TCR_DataLose:
        if(!IsExplicit)
            CM.Log(CMT_TypeConvDataLose, Node->Line, From->ToString(IsFromNilable).CStr(), To->ToString(IsToNilable).CStr());
        break;
    case TCR_Unsafe:
        if(!IsExplicit)
            CM.Log(CMT_TypeConvUnSafe, Node->Line, From->ToString(IsFromNilable).CStr(), To->ToString(IsToNilable).CStr());
        break;
    case TCR_NoWay:
        CM.Log(CMT_TypeConvNoWay, Node->Line, From->ToString(IsFromNilable).CStr(), To->ToString(IsToNilable).CStr());
        break;
    default:
        break;
    }

    if(SaveConvertInfo != false && Node->Is<AExpr>())
    {
        SPtr<AExpr> ExprNode = Node.PtrAs<AExpr>();
        if(Result == TCR_DataLose || Result == TCR_Unsafe)
        {
            ExprNode->UsedAsType = To;
            ExprNode->IsUsedAsNilable = IsToNilable;
        }
    }

    return Result;

}


TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefClass(SPtr<ClassType> Class, OLString Name, ABase* Node, bool UseTypeName, bool UseColon)
{   
    FindMemberResult Member =  Class->FindMember(Name, true);
    if(Member.FromClass == nullptr)
    {
        CM.Log(CMT_NoMember, Node->Line, Class->Name.CStr(), Name.CStr());
        if(Setting.AllowImplicitAny)
        {
            return DerefResult{IntrinsicType::CreateFromRaw(IT_any), false, true};
        }
        else
        {
            return DerefResult{nullptr, false, true};
        }
    }
    else
    {
        if(Member.IsClassMember == true)
        {
            bool IsCtor = false;
            // private restriction
            if(Class->DeclNode != nullptr && Class->DeclNode->IsParentOf(Node) == false)
            {
                if((Member.FromClass->Flags & CMF_Public) == 0)
                {
                    if((Member.FromClass->Flags & CMF_Protected) == 0 || !Class->IsBaseType(Member.FromClass->Owner.Lock().Get()))
                    {
                        CM.Log(CMT_PrivateMember, Node->Line,  Name.CStr(), Class->Name.CStr());
                    }
                    
                }

            }
            // ClassName:Member  not allowed
            if(UseTypeName && UseColon)
            {
                CM.Log(CMT_NoColonForTypeName, Node->Line);
            }
            // ClassName.Member  for static method, or static var, or constructor
            else if(UseTypeName && !UseColon)
            {
                if((Member.FromClass->Flags & CMF_Static) == 0
                    && (Member.FromClass->Flags & CMF_Constructor) == 0)
                    CM.Log(CMT_DotForStatic, Node->Line, Name.CStr(), Class->Name.CStr());

                if(Member.FromClass->Flags & CMF_Constructor)
                {
                    IsCtor = true;
                }
            }
            // Inst:Member  for non-static method, or super ctor call in sub ctor
            else if(!UseTypeName && UseColon)
            {
                if(  Member.FromClass->Type != Member_Function 
                    || (Member.FromClass->Flags & CMF_Static) != 0 )
                    {
                        CM.Log(CMT_ColonForNonStaticMethod, Node->Line, Name.CStr(), Class->Name.CStr());
                    }
            }
            // Inst.Member  for non-static var
            else if(!UseTypeName && !UseColon)
            {
                if(  Member.FromClass->Type != Member_Variant 
                    || (Member.FromClass->Flags & CMF_Static) != 0 )
                {
                    CM.Log(CMT_DotForNonStaticVar, Node->Line, Name.CStr(), Class->Name.CStr());
                }
            }


            // deref info update
            SPtr<Reference> Ref = CurrScope->FindRefByNode(Node);
            OL_ASSERT(Ref != nullptr);
            Ref->DeclScope = Member.FromClass->Owner->InsideScope;
            Ref->Decl = Member.FromClass->Decl; //Ref->DeclScope->FindDeclByNode(Member.FromClass->DeclTypeDesc.Lock()->DeclNode.Get());

            return DerefResult{Member.FromClass->DeclTypeDesc.Lock(), (Member.FromClass->Flags & CMF_Const) == 0 ? false : true, Member.FromClass->IsNilable};
        }
        else
        {
            // deref info update
            SPtr<Reference> Ref = CurrScope->FindRefByNode(Node);
            OL_ASSERT(Ref != nullptr);
            Ref->DeclScope = Member.FromInterface->Owner->InsideScope;
            Ref->Decl =  Member.FromInterface->Decl; // Ref->DeclScope->FindDeclByNode(Member.FromInterface->Func.Lock()->DeclNode.Get());
            
            // Interface does not have variable member, so no nilable ones
            return DerefResult{Member.FromInterface->Func.Lock(), (Member.FromInterface->Flags & CMF_Const) == 0 ? false : true, false};

        }
    }
}

SPtr<TypeDescBase> TypeDerivationVisitor::DerefMap(SPtr<MapType> Map, OLString Name, ABase* Node)
{
    if(Map->KeyType->ActuallyIs<IntrinsicType>() == false
        || Map->KeyType->ActuallyAs<IntrinsicType>()->Type != IT_string)
    {
        CM.Log(CMT_MapNeedStringKey, Node->Line);
    }

    return Map->ValueType.Lock();


}

TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefInterface(SPtr<InterfaceType> Interface, OLString Name, ABase* Node)
{
    // Interface does not have variable member, so no nilable ones

    InterfaceMember* Member =  Interface->FindMember(Name, true);
    if(Member == nullptr)
    {
        CM.Log(CMT_NoMember, Node->Line, Interface->Name.CStr(), Name.CStr());
        if(Setting.AllowImplicitAny)
        {
            return DerefResult{IntrinsicType::CreateFromRaw(IT_any), false, false};
        }
        else
        {
            return DerefResult{nullptr, false, false};
        }
    }
    else
    {
        // deref info update
        SPtr<Reference> Ref = CurrScope->FindRefByNode(Node);
        OL_ASSERT(Ref != nullptr);
        Ref->DeclScope = Member->Owner->InsideScope;
        Ref->Decl = Ref->DeclScope->FindDeclByNode(Member->Func.Lock()->DeclNode.Get());
        return DerefResult{Member->Func.Lock(), (Member->Flags & CMF_Const) ? true : false, false};
    }
}
TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefIntrinsic(SPtr<IntrinsicType> Intrinsic, OLString Name, ABase* Node)
{
    BuiltinLib& Lib = BuiltinLib::GetInst();
    SPtr<ClassType> BuiltinClass = Lib.GetIntrinsicClass(Intrinsic->Type);
    if(BuiltinClass == nullptr)
    {
        return DerefResult{nullptr, false, false};
    }

    return DerefClass(BuiltinClass, Name, Node, false, true);
    
}

TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefEnum(SPtr<EnumType> Enum, OLString Name, ABase* Node, bool ByTypeName)
{
    BuiltinLib& Lib = BuiltinLib::GetInst();
    SPtr<ClassType> EnumBaseClass = Lib.EnumBaseClass;
    if(ByTypeName)
    {
        if(Enum->HasItem(Name))
            return DerefResult{Enum, true, false};

        DerefResult BaseDeref = DerefClass(EnumBaseClass, Name, Node, true, false);
        if(BaseDeref.Type == nullptr)
            CM.Log(CMT_NoEnumItem, Node->Line, Name.CStr(), Enum->Name.CStr());
        else
            return BaseDeref;
    }
    else
    {
        DerefResult BaseDeref = DerefClass(EnumBaseClass, Name, Node, false, true);
        if(BaseDeref.Type == nullptr)
            CM.Log(CMT_NoEnumItem, Node->Line, Name.CStr(), Enum->Name.CStr());
        else
            return BaseDeref;
    }
    return DerefResult{nullptr, false, false};;
}


bool TypeDerivationVisitor::MatchFuncCall(SPtr<FuncSigniture> Func, int RetIndex, SPtr<AExpr> Node)
{
    OLList<TypeDeriContex> StackTypes;
    ExpandStackTuple(RetIndex, StackTypes);

    //int InStackParams = DeriStack.Count() - RetIndex - 2;
    int InStackParams = StackTypes.Count() - 1;
    int MaxRequiredParams = Func->MaxParamRequire();
    int MinRequireParams = Func->MinParamRequire();
    bool HasVariableParam = Func->HasVariableParam();


    bool OK = true;
    if(MaxRequiredParams >= 0 && InStackParams > MaxRequiredParams)
    {
        CM.Log(CMT_TooManyParam, Node->Line, Func->ToString(false).CStr());
        OK = false;
    }
    else if(InStackParams < MinRequireParams)
    {
        CM.Log(CMT_TooFewParam, Node->Line, Func->ToString(false).CStr());
        OK = false;
    }
    else
    {
        int p = 0;

        for(int i = 1; i < StackTypes.Count(); i++)
        {
            FuncParamDesc& FuncParam = Func->Params[p];
            SPtr<TypeDescBase> ParamType = Func->Params[p].Type.Lock();
            bool ParamIsNilable = Func->Params[p].IsNilable;
            TypeDeriContex& CurrCtx = StackTypes[i]; 
            if(ParamType->ActuallyIs<VariableParamHolder>())
            {
                if(CurrCtx.PrimaryType->ActuallyIs<VariableParamHolder>() == false)
                    ParamType = ParamType->ActuallyAs<VariableParamHolder>()->ParamType.Lock();
            }
            else
            {
                p++;
            }
            
            if(MatchType(CurrCtx.PrimaryType, CurrCtx.IsNilable, ParamType, ParamIsNilable, CurrCtx.Node, false) == TCR_NoWay)
            {
                CM.Log(CMT_ParamTypeError, Node->Line, (p+1));
                OK = false;
            }
            if(CurrCtx.IsConst && (FuncParam.Flags & FPF_Const) == 0 && CurrCtx.PrimaryType->IsRefType())
            {
                CM.Log(CMT_ConstRestrictCall, Node->Line, (p+1));
                OK = false;
            }

        }
    }

    if(Func->CtorOwnerType != nullptr)
    {
        DeriStack.PopTo(RetIndex);
        DeriStack[RetIndex].PrimaryType = Func->CtorOwnerType.Lock();
        //Class constructor never return nilable
        DeriStack[RetIndex].IsNilable = false;

        Node->ExprType = Func->CtorOwnerType;
        Node->IsNilable = false;
    }
    else
    {
        DeriStack.PopTo(RetIndex);
        DeriStack[RetIndex].PrimaryType = Func->UniqueReturnType.Lock();
        DeriStack[RetIndex].IsNilable = Func->UniqueReturnIsNilable;

        Node->ExprType = Func->UniqueReturnType;
        Node->IsNilable = Func->UniqueReturnIsNilable;
    }
    return OK;
}

void TypeDerivationVisitor::ExpandStackTuple(int BaseIndex, OLList<TypeDeriContex>& OutList)
{
    for(int i = BaseIndex + 1; i < DeriStack.Count(); i++)
    {
        if(DeriStack[i].PrimaryType->ActuallyIs<TupleType>())
        {
            SPtr<TupleType> Tuple = DeriStack[i].PrimaryType->ActuallyAs<TupleType>();
            for(int k = 0; k < Tuple->Subtypes.Count(); k ++)
            {
                OutList.Add(TypeDeriContex(
                    DeriStack[i].Node, Tuple->Subtypes[k].Type.Lock(), DeriStack[i].IsConst));
                OutList.Top().IsNilable = Tuple->Subtypes[k].IsNilable;
            }
        }
        else
        {
            OutList.Add(DeriStack[i]);
            OutList.Top().IsNilable = DeriStack[i].IsNilable;
        }
    }
}


OLString   TypeDerivationVisitor::GetDump()
{
    OLString Ret;
    for(int i = 0; i < DeriStack.Count(); i++)
    {
        TypeDeriContex& Curr = DeriStack[DeriStack.Count() - 1 - i];
        Ret.AppendF(T("    %s : [%s (%d, %d)] - %s\n")
            , Curr.IsType ? T("Type") : T("Value")
            , Curr.Node->GetType().Name
            , Curr.Node->Line.Line, Curr.Node->Line.Col
            , Curr.PrimaryType == nullptr? T("nullptr") : Curr.PrimaryType->ToString(Curr.IsNilable).CStr());
    }
    return Ret;
}

}