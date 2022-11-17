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

#define ASSERT_CHILD_NUM(index, count) OL_ASSERT((count) == (DeriStack.Count() - 1 - (index)))

namespace OL
{

TypeDeriContex::TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType,  bool InIsConst )
    : Node(InNode), PrimaryType(InPrimaryType), IsType(false), IsConst(InIsConst), IsVariableParamRef(false)
{

}

TypeDeriContex::TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType , bool InIsConst, bool InIsType )
    : Node(InNode), PrimaryType(InPrimaryType), IsType(InIsType), IsConst(InIsConst), IsVariableParamRef(false)
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
    if(Node->FirstUniOp != UO_None)
    {
        CurrType = DeriStack[Index + 1].PrimaryType->AcceptUniOp(Node->FirstUniOp);
        if(CurrType == nullptr)
        {
            CM.Log(CMT_UniopNotMatch, Node->Line, ASubexpr::UniopToString(Node->FirstUniOp), DeriStack[Index + 1].PrimaryType->ToString().CStr());
            return VS_Stop;
        }
    }
    else
    {
        CurrType = DeriStack[Index + 1].PrimaryType;
    }

    for(int i = 1; i < Node->OperandList.Count(); i++)
    {
        SPtr<TypeDescBase> Target = DeriStack[Index + 1 + i].PrimaryType;
        SPtr<TypeDescBase> Result = CurrType->AcceptBinOp(Node->OperandList[i].Op, Target);
        if(Result == nullptr)
        {
            CM.Log(CMT_BinopNotMatch, Node->Line, ASubexpr::BinopToString(Node->OperandList[i].Op), CurrType->ToString().CStr(), Target->ToString().CStr());
            return VS_Stop;
        }

        CurrType = Result;
    }
    
    if(CurrType == nullptr)
    {

    }

    DeriStack[Index].PrimaryType = CurrType;
    DeriStack.PopTo(Index);
    Node->ExprType = CurrType;
    
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

    SPtr<TypeDescBase> TargetType = CurrScope->FindTypeByNode(Node->TypeInfo.Get(), true);

    TypeDeriContex& Top = DeriStack.Top();
    bool Ret = CheckType(Top.PrimaryType, TargetType, Node.Get(), true);
    bool IsConst = Top.IsConst;
    DeriStack.Pop();


    DeriStack.Top().PrimaryType = TargetType;
    DeriStack.Top().IsConst = IsConst;
    Node->ExprType = TargetType;
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
    if(DotTarget.PrimaryType->Is<MapType>())
    {
        MemberType = DerefMap(DotTarget.PrimaryType.PtrAs<MapType>(), Node->Field, Node.Get());
        IsConst = DotTarget.IsConst;
    }
    else if(DotTarget.PrimaryType->Is<ClassType>())
    {
        DerefResult Deref = DerefClass(DotTarget.PrimaryType.PtrAs<ClassType>(), Node->Field, Node.Get(), DotTarget.IsType, false);
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
    }
    else if(DotTarget.PrimaryType->Is<InterfaceType>())
    {
        DerefResult Deref =  DerefInterface(DotTarget.PrimaryType.PtrAs<InterfaceType>(), Node->Field, Node.Get());
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;  
    }
    else if(DotTarget.PrimaryType->Is<EnumType>() && DotTarget.IsType == true)
    {
        if(DerefEnum(DotTarget.PrimaryType.PtrAs<EnumType>(), Node->Field, Node.Get()) == true)
        {
            MemberType = DotTarget.PrimaryType;
        }
    }

    if(MemberType != nullptr)
    {
        DeriStack.Top().PrimaryType = MemberType;
        DeriStack.Top().IsConst = DotTarget.IsConst ? true : IsConst;
        Node->ExprType = MemberType;
    }
    else
    {
        if(Setting.AllowImplicitAny)
        {    
            DeriStack.Top().PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
            Node->ExprType = DeriStack.Top().PrimaryType;
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

    if(Target.PrimaryType->Is<ArrayType>())
    {
        if(CheckType(Field.PrimaryType, IntrinsicType::CreateFromRaw(IT_int), Node.Get(), false) == false)
        {
            CM.Log(CMT_ArrayNeedIntIndex, Node->Line);
        }
        DeriStack.Top().PrimaryType = Target.PrimaryType->As<ArrayType>()->ElemType.Lock();
        DeriStack.Top().IsConst = Target.IsConst;
        Node->ExprType = DeriStack.Top().PrimaryType;
    }
    else if(Target.PrimaryType->Is<MapType>())
    {
        if(CheckType(Field.PrimaryType, Target.PrimaryType->As<MapType>()->KeyType.Lock(), Node.Get(), false) == false)
        {
            CM.Log(CMT_MapKeyTypeMismatch, Node->Line);
        }
        DeriStack.Top().PrimaryType = Target.PrimaryType->As<ArrayType>()->ElemType.Lock();
        DeriStack.Top().IsConst = Target.IsConst;
        Node->ExprType = DeriStack.Top().PrimaryType;
    }
    else
    {
        CM.Log(CMT_InvalidBracket, Node->Line, Target.PrimaryType->ToString().CStr());
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

    if(FuncBase->Is<FuncSigniture>() == false)
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
            DeriStack.PopTo(Index);
            return VS_Continue;
        }
    }

    SPtr<FuncSigniture> Func = FuncBase.PtrAs<FuncSigniture>();
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

    SPtr<TypeDescBase> MemberType = nullptr;
    bool IsConst = false;
    if(FuncOwner->Is<MapType>())
    {
        MemberType = DerefMap(FuncOwner.PtrAs<MapType>(), Node->NameAfter, Node.Get());
    }
    else if(FuncOwner->Is<ClassType>())
    {
        DerefResult Deref =  DerefClass(FuncOwner.PtrAs<ClassType>(), Node->NameAfter, Node.Get(), DeriStack[Index + 1].IsType, true);
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
    }
    else if(FuncOwner->Is<InterfaceType>())
    {
        DerefResult Deref =  DerefInterface(FuncOwner->As<InterfaceType>(), Node->NameAfter, Node.Get());
        MemberType = Deref.Type;
        IsConst = Deref.IsConst;
    }
    else if(FuncOwner->IsAny())
    {
        // Any type owner, Do not validate
        DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
        DeriStack.PopTo(Index);
        return VS_Continue;
    }
    if(MemberType == nullptr)
    {
        ERROR(LogCompile, T("Internal error while dereferencing member %s (%d,%d)"), Node->NameAfter.CStr(), Node->Line.Line, Node->Line.Col);
        DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
        DeriStack.PopTo(Index);
        return VS_Continue;

    }

    if(MemberType->Is<FuncSigniture>() == false)
    {
        if(MemberType->IsAny())
        {
            // Any type, Do not validate
            DeriStack[Index].PrimaryType = IntrinsicType::CreateFromRaw(IT_any);
            DeriStack.PopTo(Index);
            return VS_Continue;
        }
        else
        {
            CM.Log(CMT_RequirFunc, Node->Line);
            return VS_Stop;
        }
    }

    if(!IsConst && IsOwnerConst)
    {
        CM.Log(CMT_ConstRestrictOwner, Node->Line);
        return VS_Stop;
    }

    bool Match = MatchFuncCall(MemberType.PtrAs<FuncSigniture>(), Index, Node);

    return VS_Continue;

}

EVisitStatus TypeDerivationVisitor::BeginVisit(SPtr<AFuncExpr> Node)
{
    SPtr<TypeDescBase> FuncBase = CurrScope->FindTypeByNode(Node.Get(), false);
    OL_ASSERT(FuncBase->Is<FuncSigniture>());

    DeriStack.Add(TypeDeriContex(Node, FuncBase, false));
    IndexStack.Add(DeriStack.Count() - 1);
    Node->ExprType = FuncBase;

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
    Node->ExprType = CtorType;
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
    Node->ExprType = Const;

    return VS_Continue;
}

EVisitStatus TypeDerivationVisitor::Visit(SPtr<AVarRef> Node)
{
    SPtr<Reference> Ref = CurrScope->FindRefByNode(Node.Get());
    OL_ASSERT(Ref != nullptr);

    if(Ref->Decl == nullptr)
    {
        DeriStack.Add(TypeDeriContex(Node, IntrinsicType::CreateFromRaw(IT_any), false));
        CM.Log(CMT_ImplicitGlobal, Node->Line, Node->VarName.CStr());
        Node->ExprType = IntrinsicType::CreateFromRaw(IT_any, true);
    }
    else
    {
        
        if(SymbolScope::IsResolvedType(Ref->Type))
        {
            SPtr<TypeDescBase> TypeDesc = Ref->DeclScope->FindTypeByNode(Ref->Decl->DeclNode.Get(), false);
            DeriStack.Add(TypeDeriContex(Node,TypeDesc, false, true));
        }
        else
        {
            // Check if this is in a ctor and accessing class member var.
            // In this case const member can be treated as non-const
            bool IsConst = Ref->Decl->IsConst;
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
        }
    }
    Node->ExprType = DeriStack.Top().PrimaryType;

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
    Node->ExprType = SuperClass;
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
    Node->ExprType = Class;
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

    Node->ExprType = DeriStack.Top().PrimaryType;

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
        
        CheckType(InitValType, Decl->ValueTypeDesc.Lock(), Node->Inits[i].Get(), false );
        
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
        CheckType(DeriStack[Index + 1].PrimaryType, IntrinsicType::CreateFromRaw(IT_int), Node->ValueExpr.Get(), false);
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
        SPtr<TypeDescBase> LValueType = DeriStack[Index + 1 + i].PrimaryType;
        if(DeriStack[Index + 1 + i].IsConst == true)
        {
            CM.Log(CMT_ConstRestrictAssign, Node->Line);
        }
        SPtr<TypeDescBase> RValueType = DeriStack[Index + Node->LeftExprs.Count() + 1 + i].PrimaryType;
        CheckType(RValueType, LValueType, Node->LeftExprs[i].Get(), false);
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

    CheckType(DeriStack[Index + 1].PrimaryType, IntrinsicType::CreateFromRaw(IT_bool), Node->Condition.Get(), false);
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
    if(IteratorType->Is<ArrayType>())
    {
        if(Node->VarList.Count() != 1)
        {
            CM.Log(CMT_ArrayIterMismatch, Node->Iterator->Line);
        }
        else
        {
            SPtr<Declearation> DeclInfo = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            CheckType(IteratorType->As<ArrayType>()->ElemType.Lock(), DeclInfo->ValueTypeDesc.Lock(), Node->Iterator.Get(), false);
        }
    }
    else if(IteratorType->Is<MapType>())
    {
        SPtr<MapType> Map = IteratorType->As<MapType>();
        if(Node->VarList.Count() != 2)
        {
            CM.Log(CMT_MapIterMismatch, Node->Iterator->Line);
        }
        else
        {
            SPtr<Declearation> KeyDeclInfo = InsideScope->FindDeclByNode(Node->VarList[0].Get());
            SPtr<Declearation> ValueDeclInfo = InsideScope->FindDeclByNode(Node->VarList[1].Get());

            CheckType(Map->KeyType.Lock(), KeyDeclInfo->ValueTypeDesc.Lock(), Node->VarList[0].Get(), false);
            CheckType(Map->ValueType.Lock(), ValueDeclInfo->ValueTypeDesc.Lock(), Node->VarList[1].Get(), false);
        }
    }
    else if(IteratorType->Is<ConstructorType>())
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

        ETypeValidation Result = IteratorType->As<ConstructorType>()->ValidateIterator(Type1, Type2);
        if(Result == TCR_NoWay)
        {
            CM.Log(CMT_IterMismatch, Node->Iterator->Line);
        }
    }
    else if(IteratorType->IsAny())
    {

    }
    else if(IteratorType->Is<TupleType>())
    {
        SPtr<TupleType> Tuple = IteratorType.PtrAs<TupleType>();

        auto ValidateIteratorFun = [&]() ->bool
        {
            if(Tuple->Subtypes.Count() != 3)
            {
                // error
                CM.Log(CMT_IterTupleNum, Node->Line);
                return false;
            }

            if(Tuple->Subtypes[0].Type->Is<FuncSigniture>() == false)
            {
                // error
                CM.Log(CMT_IterEntryType, Node->Line);
                return false;
            }

            SPtr<FuncSigniture> IterFun = Tuple->Subtypes[0].Type.Lock().PtrAs<FuncSigniture>();
            if(IterFun->Params.Count() != 2)
            {
                // error
                CM.Log(CMT_IterFuncType, Node->Line);
                return false;
            }

            // First param must be compatible with the second in tuple
            SPtr<TypeDescBase> StableVarType = Tuple->Subtypes[1].Type.Lock();
            bool Result = CheckType(StableVarType, IterFun->Params[0].Type.Lock(), Tuple->DeclNode.Lock().Get(), false);
            if(!Result)
            {
                // error
                CM.Log(CMT_IterTargetType, Node->Line);
                return false;
            }

            // Second param must be compatible with indexing value(third in tuple)
            SPtr<TypeDescBase> IndexingVarType = Tuple->Subtypes[2].Type.Lock();
            Result = CheckType(IndexingVarType, IterFun->Params[1].Type.Lock(), Tuple->DeclNode.Lock().Get(), false);
            if(!Result)
            {
                // error
                CM.Log(CMT_IterIndexType, Node->Line);
                return false;
            }

            // Return value contains no less than Node->VarList
            if( IterFun->Returns.Count() <  Node->VarList.Count())
            {
                // error
                CM.Log(CMT_IterItemNum, Node->Line);
                return false;
            }

            // first return value must be compatible with indexing type
            Result = CheckType(IndexingVarType, IterFun->Returns[0].Type.Lock(), Tuple->DeclNode.Lock().Get(), false);
            if(!Result)
            {
                // error
                CM.Log(CMT_IterFuncIndexType, Node->Line);
                return false;
            }

            for(int i = 0; i < Node->VarList.Count(); i++)
            {
                SPtr<Declearation> DeclInfo = InsideScope->FindDeclByNode(Node->VarList[i].Get());
                Result = CheckType(IterFun->Returns[i].Type.Lock(), DeclInfo->ValueTypeDesc.Lock(), Tuple->DeclNode.Lock().Get(), false);
                if(Result == false)
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
    CheckType(DeriStack[Index + 1].PrimaryType, IntType, DeriStack[Index + 1].Node.Get(), false);
    CheckType(DeriStack[Index + 2].PrimaryType, IntType, DeriStack[Index + 2].Node.Get(), false);
    if(ExpectNum == 3)
    {
        CheckType( DeriStack[Index + 3].PrimaryType, IntType, DeriStack[Index + 3].Node.Get(), false);
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
        SPtr<TypeDescBase> ExprType = StackTypes[i].PrimaryType; // DeriStack[Index + 1 + i].PrimaryType;
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
        if(Decl->ValueTypeDesc->IsImplicitAny() && ExprType->Is<VariableParamHolder>() == false)
            Decl->ValueTypeDesc = ExprType->DeduceLValueType(CurrScope);
        else
            CheckType(ExprType, Decl->ValueTypeDesc.Lock(), Node->Decls[i].Get(), false);
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
        SPtr<TypeDescBase> ExprType = StackTypes[i].PrimaryType; // DeriStack[Index + 1 + i].PrimaryType;
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node->Decls[i].Get());
        if(Decl->ValueTypeDesc->IsImplicitAny() && ExprType->Is<VariableParamHolder>() == false)
            Decl->ValueTypeDesc = ExprType->DeduceLValueType(CurrScope);
        else
            CheckType(ExprType, Decl->ValueTypeDesc.Lock(), Node->Decls[i].Get(), false);
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
    CheckType(DeriStack[Index + 1].PrimaryType, BoolType,  DeriStack[Index + 1].Node.Get(), false);

    for(int i = 0; i < Node->ElseIfBlocks.Count(); i++)
    {
        SPtr<TypeDescBase> CondType = DeriStack[Index + 2 + i].PrimaryType;
        CheckType( DeriStack[Index + 2 + i].PrimaryType, BoolType, DeriStack[Index + 2 + i].Node.Get(), false);
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

    CheckType(DeriStack[Index + 1].PrimaryType, IntrinsicType::CreateFromRaw(IT_bool), DeriStack[Index + 1].Node.Get(), false );
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
            
            SPtr<TypeDescBase> StatType = DeriStack[Index + 1 + i].PrimaryType;
            CheckType(StatType, DeclType, DeriStack[Index + 1 + i].Node.Get(), false);
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

    CheckType(DeriStack[Index + 1].PrimaryType, IntrinsicType::CreateFromRaw(IT_bool), DeriStack[Index + 1].Node.Get(), false );
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
        Node->ExprType = ParamDesc.Type;
    }
    else
    {
        OL_ASSERT(0 && T("Invalid use of '...', should be checked in previous steps"));
    }
    return VS_Continue;
}

bool TypeDerivationVisitor::CheckType(SPtr<TypeDescBase> From, SPtr<TypeDescBase> To, ABase* Node, bool IsExplicit)
{
    ETypeValidation Result = From->ValidateConvert(To, IsExplicit);
    switch (Result)
    {
    case TCR_OK:
        return true;
    case TCR_DataLose:
        CM.Log(CMT_TypeConvDataLose, Node->Line, From->ToString().CStr(), To->ToString().CStr());
        return true;
    case TCR_Unsafe:
        CM.Log(CMT_TypeConvUnSafe, Node->Line, From->ToString().CStr(), To->ToString().CStr());
        return true;
    case TCR_NoWay:
        CM.Log(CMT_TypeConvNoWay, Node->Line, From->ToString().CStr(), To->ToString().CStr());
        return false;
    default:
        break;
    }

    return false;

}


TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefClass(SPtr<ClassType> Class, OLString Name, ABase* Node, bool UseTypeName, bool UseColon)
{   
    FindMemberResult Member =  Class->FindMember(Name, true);
    if(Member.FromClass == nullptr)
    {
        CM.Log(CMT_NoMember, Node->Line, Class->Name.CStr(), Name.CStr());
        if(Setting.AllowImplicitAny)
        {
            return DerefResult{IntrinsicType::CreateFromRaw(IT_any), false};
        }
        else
        {
            return DerefResult{nullptr, false};
        }
    }
    else
    {
        if(Member.IsClassMember == true)
        {
            bool IsCtor = false;
            // private restriction
            if(Class->DeclNode->IsParentOf(Node) == false)
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

            return DerefResult{Member.FromClass->DeclTypeDesc.Lock(), (Member.FromClass->Flags & CMF_Const) == 0 ? false : true};
        }
        else
        {
            // deref info update
            SPtr<Reference> Ref = CurrScope->FindRefByNode(Node);
            OL_ASSERT(Ref != nullptr);
            Ref->DeclScope = Member.FromInterface->Owner->InsideScope;
            Ref->Decl =  Member.FromInterface->Decl; // Ref->DeclScope->FindDeclByNode(Member.FromInterface->Func.Lock()->DeclNode.Get());
            
            return DerefResult{Member.FromInterface->Func.Lock(), (Member.FromInterface->Flags & CMF_Const) == 0 ? false : true};

        }
    }
}

SPtr<TypeDescBase> TypeDerivationVisitor::DerefMap(SPtr<MapType> Map, OLString Name, ABase* Node)
{
    if(Map->KeyType->Is<IntrinsicType>() == false
        || Map->KeyType->As<IntrinsicType>()->Type != IT_string)
    {
        CM.Log(CMT_MapNeedStringKey, Node->Line);
    }

    return Map->ValueType.Lock();


}

TypeDerivationVisitor::DerefResult TypeDerivationVisitor::DerefInterface(SPtr<InterfaceType> Interface, OLString Name, ABase* Node)
{
    InterfaceMember* Member =  Interface->FindMember(Name, true);
    if(Member == nullptr)
    {
        CM.Log(CMT_NoMember, Node->Line, Interface->Name.CStr(), Name.CStr());
        if(Setting.AllowImplicitAny)
        {
            return DerefResult{IntrinsicType::CreateFromRaw(IT_any), false};
        }
        else
        {
            return DerefResult{nullptr, false};
        }
    }
    else
    {
        // deref info update
        SPtr<Reference> Ref = CurrScope->FindRefByNode(Node);
        OL_ASSERT(Ref != nullptr);
        Ref->DeclScope = Member->Owner->InsideScope;
        Ref->Decl = Ref->DeclScope->FindDeclByNode(Member->Func.Lock()->DeclNode.Get());
        return DerefResult{Member->Func.Lock(), (Member->Flags & CMF_Const) ? true : false};
    }
}

bool TypeDerivationVisitor::DerefEnum(SPtr<EnumType> Enum, OLString Name, ABase* Node)
{
    if(Enum->HasItem(Name))
        return true;

    CM.Log(CMT_NoEnumItem, Node->Line, Name.CStr(), Enum->Name.CStr());
    return false;
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
        CM.Log(CMT_TooManyParam, Node->Line, Func->ToString().CStr());
        OK = false;
    }
    else if(InStackParams < MinRequireParams)
    {
        CM.Log(CMT_TooFewParam, Node->Line, Func->ToString().CStr());
        OK = false;
    }
    else
    {
        int p = 0;
        //for(int i = RetIndex + 2; i < DeriStack.Count(); i++, p++)
        for(int i = 1; i < StackTypes.Count(); i++)
        {
            FuncParamDesc& FuncParam = Func->Params[p];
            SPtr<TypeDescBase> ParamType = Func->Params[p].Type.Lock();
            TypeDeriContex& CurrCtx = StackTypes[i]; // DeriStack[i]
            if(ParamType->Is<VariableParamHolder>())
            {
                if(CurrCtx.PrimaryType->Is<VariableParamHolder>() == false)
                    ParamType = ParamType->As<VariableParamHolder>()->ParamType.Lock();
            }
            else
            {
                p++;
            }
            
            if(CheckType(CurrCtx.PrimaryType, ParamType, Node.Get(), false) == false)
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
        Node->ExprType = Func->CtorOwnerType;
    }
    else
    {
        DeriStack.PopTo(RetIndex);
        DeriStack[RetIndex].PrimaryType = Func->UniqueReturnType.Lock();
        Node->ExprType = Func->UniqueReturnType;
    }
    return OK;
}

void TypeDerivationVisitor::ExpandStackTuple(int BaseIndex, OLList<TypeDeriContex>& OutList)
{
    for(int i = BaseIndex + 1; i < DeriStack.Count(); i++)
    {
        if(DeriStack[i].PrimaryType->Is<TupleType>())
        {
            SPtr<TupleType> Tuple = DeriStack[i].PrimaryType.PtrAs<TupleType>();
            for(int k = 0; k < Tuple->Subtypes.Count(); k ++)
            {
                OutList.Add(TypeDeriContex(
                    DeriStack[i].Node, Tuple->Subtypes[k].Type.Lock(), DeriStack[i].IsConst));
            }
        }
        else
        {
            OutList.Add(DeriStack[i]);
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
            , Curr.PrimaryType == nullptr? T("nullptr") : Curr.PrimaryType->ToString().CStr());
    }
    return Ret;
}

}