/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TypeDescBase.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
#include "AFuncBody.h"
#include "AstInclude.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "TupleType.h"
#include "VoidHolder.h"
#include "ArrayType.h"
#include "MapType.h"

namespace OL
{

STRUCT_RTTI_BEGIN(FuncParamDesc)
    RTTI_MEMBER(Type, MF_External)
    RTTI_MEMBER(Flags)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(IsResolved)
    RTTI_STRUCT_MEMBER(Line, CodeLineInfo)
STRUCT_RTTI_END(FuncParamDesc)

STRUCT_RTTI_BEGIN(FuncReturnDesc)
    RTTI_MEMBER(Type, MF_External)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(IsResolved)
    RTTI_STRUCT_MEMBER(Line, CodeLineInfo)
STRUCT_RTTI_END(FuncReturnDesc)


RTTI_BEGIN_INHERITED(FuncSigniture, TypeDescBase)
    RTTI_STRUCT_MEMBER(Params, FuncParamDesc)
    RTTI_STRUCT_MEMBER(Returns, FuncReturnDesc)
    RTTI_MEMBER(HasThis)
    RTTI_MEMBER(UnresolvedThisType)
    RTTI_MEMBER(ThisType, MF_External)
RTTI_END(FuncSigniture)


FuncSigniture::FuncSigniture()
: HasThis(false)
{

}

FuncSigniture::~FuncSigniture()
{

}

void FuncSigniture::SetThis(SPtr<TypeDescBase> InThisType)
{
    HasThis = true;
    ThisType = InThisType;
}

void FuncSigniture::SetUnresolvedThis(OLString InThisTypeName)
{
    HasThis = true;
    UnresolvedThisType = InThisTypeName;
}

void FuncSigniture::SetCtorOwner(SPtr<TypeDescBase> InCtorOwner)
{
    CtorOwnerType = InCtorOwner;
}


void FuncSigniture::AddParam(OLString &UnresolvedName, bool IsConst, bool IsVariableParam, bool IsOptional,  CodeLineInfo& Line)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.UnresolvedTypeName = UnresolvedName;
    NewDesc.IsResolved = false;
    NewDesc.Line = Line;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
}

void FuncSigniture::AddParam(EIntrinsicType Type, bool IsConst, bool IsVariableParam, bool IsOptional,  CodeLineInfo& Line)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.Type = IntrinsicType::CreateFromRaw(Type);
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.IsResolved = true;
    NewDesc.Line = Line;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
}

void FuncSigniture::AddParam(SPtr<TypeDescBase> Type, bool IsConst, bool IsVariableParam, bool IsOptional,  CodeLineInfo& Line)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.Type = Type;
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.IsResolved = true;
    NewDesc.Line = Line;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
}

void FuncSigniture::AddReturn(SPtr<TypeDescBase> Type, CodeLineInfo& Line)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = true;
    NewDesc.Type = Type;
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.Line = Line;
}

void FuncSigniture::AddReturn(OLString& UnresolvedName, CodeLineInfo& Line)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = false;
    
    NewDesc.UnresolvedTypeName = UnresolvedName;
    NewDesc.Line = Line;
}

void FuncSigniture::AddReturn(EIntrinsicType Type, CodeLineInfo& Line)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = true;
    NewDesc.Type = IntrinsicType::CreateFromRaw(Type);
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.Line = Line;
}


bool FuncSigniture::HasVariableParam()
{
    for(int i = 0; i < Params.Count(); i++)
    {
        if(Params[i].IsVariableParam)
            return true;
    }
    return false;
}

void FuncSigniture::MakeUniqueReturn(SPtr<TupleType> EmptyTuple)
{
    if(Returns.Count() > 1)
    {
        UniqueReturnType = EmptyTuple;
        for(int i = 0; i < Returns.Count(); i++)
        {
            EmptyTuple->AddSubtype(Returns[i].Type.Lock());
        }
    }
    else if(Returns.Count() == 1)
    {
        UniqueReturnType = Returns[0].Type;
    }
    else
    {
        UniqueReturnType = VoidHolder::Get();
    }
}

bool FuncSigniture::IsParamDecl(AVarDecl* Node)
{
    if(DeclNode->Is<AFuncType>())
    {
        AFuncType* FuncType = DeclNode->As<AFuncType>();
        for (int i = 0; i < FuncType->Params.Count(); i++)
        {
            if (Node == FuncType->Params[i].Get())
                return true;
        }
        return false;
    }

    SPtr<AFuncBody> Body;
    if(DeclNode->Is<AMethod>())
    {
        Body = DeclNode->As<AMethod>()->Body;
    }
    else if(DeclNode->Is<AFuncDef>())
    {
        Body = DeclNode->As<AFuncDef>()->Body;
    }
    else if(DeclNode->Is<AFuncExpr>())
    {
        Body = DeclNode->As<AFuncExpr>()->Body;
    }
    if(Body == nullptr)
        return false;

    for(int i = 0; i < Body->Params.Count(); i++)
    {
        if(Node == Body->Params[i].Get())
            return true;
    }

    return false;
}

bool FuncSigniture::IsReturnDecl(ATypeIdentity* Node)
{
    if(DeclNode->Is<AFuncType>())
    {
        AFuncType* FuncType = DeclNode->As<AFuncType>();
        for (int i = 0; i < FuncType->ReturnType.Count(); i++)
        {
            if (Node == FuncType->ReturnType[i].Get())
                return true;
        }
        return false;
    }

    SPtr<AFuncBody> Body;
    if(DeclNode->Is<AMethod>())
    {
        Body = DeclNode->As<AMethod>()->Body;
    }
    else if(DeclNode->Is<AFuncDef>())
    {
        Body = DeclNode->As<AFuncDef>()->Body;
    }
    else if(DeclNode->Is<AFuncExpr>())
    {
        Body = DeclNode->As<AFuncExpr>()->Body;
    }
    if(Body == nullptr)
        return false;

    for(int i = 0; i < Body->ReturnType.Count(); i++)
    {
        if(Node == Body->ReturnType[i].Get())
            return true;
    }

    return false;
}

void FuncSigniture::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(HasThis && ThisType == nullptr)
    {
        ThisType = Scope->FindNamedType(UnresolvedThisType, true);
        if(ThisType == nullptr)
        {
            // Error: Done
            OL_ASSERT(0 && "Cannot find 'this' type");
        }
    }

    for(int i = 0; i < Params.Count(); i++)
    {
        if(Params[i].Type == nullptr)
        {
            Params[i].Type = Scope->FindNamedType(Params[i].UnresolvedTypeName, true);
            if(Params[i].Type == nullptr)
            {
                if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
                {
                    CM.Log(CMT_NoType, Params[i].Line,  Params[i].UnresolvedTypeName.CStr());
                    Params[i].Type = IntrinsicType::CreateFromRaw(IT_any);
                    Params[i].IsResolved = true;
                }
            }
            else
            {
                Params[i].IsResolved = true;
            }
        }

    }

    for(int i = 0; i < Returns.Count(); i++)
    {
        if (Returns[i].Type == nullptr)
        {
            Returns[i].Type = Scope->FindNamedType(Returns[i].UnresolvedTypeName, true);
            if (Returns[i].Type == nullptr)
            {
                if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
                {
                    CM.Log(CMT_NoType, Returns[i].Line,  Returns[i].UnresolvedTypeName.CStr());
                    Returns[i].Type = IntrinsicType::CreateFromRaw(IT_any);
                    Returns[i].IsResolved = true;
                }
            }
            else
            {
                Returns[i].IsResolved = true;
            }
        }
    }

    if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
    {
        if(Returns.Count() > 1)
        {
            SPtr<TupleType> Tuple = new TupleType();
            Tuple->DeclNode = DeclNode;
            Scope->TypeDefs.Add(Tuple);
            MakeUniqueReturn(Tuple);
        }
        else
        {
            MakeUniqueReturn(nullptr);
        }
    }
}

ETypeValidation FuncSigniture::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(EqualsTo(Target))
        return TCR_OK;
    
    if(Target->ActuallyIs<IntrinsicType>() && Target->ActuallyAs<IntrinsicType>()->Type == IT_any)
        return TCR_OK;

    return TCR_NoWay;
}

bool FuncSigniture::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<FuncSigniture>())
    {
        SPtr<FuncSigniture> TargetFunc = Target->ActuallyAs<FuncSigniture>();
        if(Params.Count() != TargetFunc->Params.Count()
            || Returns.Count() != TargetFunc->Returns.Count())
            return false;

        if(HasThis != TargetFunc->HasThis)
            return false;
        
        if(HasThis)
        {
            if(ThisType.Lock()->EqualsTo(TargetFunc->ThisType.Lock()) == false)
                return false;
        }
        
        for(int i = 0; i < Params.Count(); i++)
        {
            if(Params[i].Flags != TargetFunc->Params[i].Flags)
                return false;
            if(Params[i].IsVariableParam != TargetFunc->Params[i].IsVariableParam)
                return false;
            if(Params[i].IsOptional != TargetFunc->Params[i].IsOptional)
                return false;
            if(Params[i].Type.Lock()->EqualsTo(TargetFunc->Params[i].Type.Lock()) == false)
                return false;
        }

        for(int i = 0; i < Returns.Count(); i++)
        {
            if(Returns[i].Type.Lock()->EqualsTo(TargetFunc->Returns[i].Type.Lock()) == false)
                return false;
        }
        return true;
    }
    return false;
}


OLString FuncSigniture::ToString()
{
    OLString Ret;
    Ret.AppendF(T("function ("));
    bool BeginOptionalParam = false;
    for(int i = 0; i < Params.Count(); i++)
    {
        OLString TypeName = T("unknown_type");
        if(Params[i].Type != nullptr)
            TypeName = Params[i].Type->ToString();

        if(Params[i].IsOptional)
        {
            BeginOptionalParam = true;
            Ret.Append(T("["));
        }
        if(i != Params.Count() - 1)
            Ret.AppendF(T("_p%d as %s%s, "), i, (Params[i].Flags & FPF_Const) ? T("const ") : T(""), TypeName.CStr());
        else
            Ret.AppendF(T("_p%d as %s%s"), i, (Params[i].Flags & FPF_Const) ? T("const ") : T(""), TypeName.CStr());
    }
    if(BeginOptionalParam)
        Ret.Append(T("]"));

    if(Returns.Count() > 0)
    {
        Ret.Append(T(") as ("));
        for(int i = 0; i < Returns.Count(); i++)
        {
            OLString TypeName = T("unknown_type");
            if(Returns[i].Type != nullptr)
                TypeName = Returns[i].Type->ToString();

            if(i != Returns.Count() - 1)
                Ret.AppendF(T("%s, "), TypeName.CStr());
            else
                Ret.AppendF(T("%s"), TypeName.CStr());
        }
        Ret.Append(T(")"));
    }
    else
    {
        Ret.Append(T(")"));
    }

    return Ret;
}
bool FuncSigniture::IsNilable()
{
    return true;
}

SPtr<TypeDescBase> FuncSigniture::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<FuncSigniture>() || Target->IsAny() || Target->IsNil()))
    {
        return IntrinsicType::CreateFromRaw(IT_bool);
    }

    return nullptr;

     
}

SPtr<TypeDescBase> FuncSigniture::AcceptUniOp(EUniOp Op)
{
    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    return nullptr;
}

int FuncSigniture::MaxParamRequire()
{
    if(Params.Count() > 0 && Params[Params.Count() - 1].IsVariableParam)
        return -1;
    return Params.Count();
}

int FuncSigniture::MinParamRequire()
{
    int RequireCount = 0;
    for(int i = 0; i < Params.Count(); i++)
    {
        if(Params[i].IsOptional == false)
            RequireCount++;
    }
    return RequireCount;
}


}