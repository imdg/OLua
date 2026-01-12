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
#include "VariableParamHolder.h"
namespace OL
{

STRUCT_RTTI_BEGIN(FuncParamDesc)
    RTTI_MEMBER(Type, MF_External)
    RTTI_MEMBER(Flags)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(IsResolved)
    RTTI_MEMBER(IsNilable)
    RTTI_STRUCT_MEMBER(SrcRange, SourceRange)
STRUCT_RTTI_END(FuncParamDesc)

STRUCT_RTTI_BEGIN(FuncReturnDesc)
    RTTI_MEMBER(Type, MF_External)
    RTTI_MEMBER(UnresolvedTypeName)
    RTTI_MEMBER(IsResolved)
    RTTI_MEMBER(IsNilable)
    RTTI_STRUCT_MEMBER(SrcRange, SourceRange)
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


void FuncSigniture::AddParam(OLString &UnresolvedName, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable,  SourceRange& SrcRange)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.UnresolvedTypeName = UnresolvedName;
    NewDesc.IsResolved = false;
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
    NewDesc.IsNilable = IsNilable || IsOptional;  // Optional param is forced to be nilable
}

void FuncSigniture::AddParam(EIntrinsicType Type, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable,  SourceRange& SrcRange)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.Type = IntrinsicType::CreateFromRaw(Type);
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.IsResolved = true;
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
    NewDesc.IsNilable = IsNilable || IsOptional; // Optional param is forced to be nilable

}

void FuncSigniture::AddParam(SPtr<TypeDescBase> Type, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable,  SourceRange& SrcRange)
{
    FuncParamDesc& NewDesc = Params.AddConstructed();
    NewDesc.Flags = IsConst ? FPF_Const : 0;
    NewDesc.Type = Type;
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.IsResolved = true;
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsVariableParam = IsVariableParam;
    NewDesc.IsOptional = IsOptional;
    NewDesc.IsNilable = IsNilable || IsOptional; // Optional param is forced to be nilable
}

void FuncSigniture::AddReturn(SPtr<TypeDescBase> Type, bool IsNilable, SourceRange& SrcRange)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = true;
    NewDesc.Type = Type;
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsNilable = IsNilable;
}

void FuncSigniture::AddReturn(OLString& UnresolvedName, bool IsNilable, SourceRange& SrcRange)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = false;
    
    NewDesc.UnresolvedTypeName = UnresolvedName;
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsNilable = IsNilable;
}

void FuncSigniture::AddReturn(EIntrinsicType Type, bool IsNilable, SourceRange& SrcRange)
{
    FuncReturnDesc& NewDesc = Returns.AddConstructed();
    NewDesc.IsResolved = true;
    NewDesc.Type = IntrinsicType::CreateFromRaw(Type);
    NewDesc.UnresolvedTypeName = T("");
    NewDesc.SrcRange = SrcRange;
    NewDesc.IsNilable = IsNilable;
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
            EmptyTuple->AddSubtype(Returns[i].Type.Lock(), Returns[i].IsNilable);
        }
        // Nilable info stored inside TupleType
        UniqueReturnIsNilable = false;
    }
    else if(Returns.Count() == 1)
    {
        UniqueReturnType = Returns[0].Type;
        UniqueReturnIsNilable = Returns[0].IsNilable;
    }
    else
    {
        UniqueReturnType = VoidHolder::Get();
        UniqueReturnIsNilable = false;
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
                    CM.Log(CMT_NoType, Params[i].SrcRange,  Params[i].UnresolvedTypeName.CStr());
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
                    CM.Log(CMT_NoType, Returns[i].SrcRange,  Returns[i].UnresolvedTypeName.CStr());
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

bool MatchParamProperty(FuncParamDesc& Src, FuncParamDesc& Dst)
{
    // Source properties
    bool SrcIsOpt = Src.IsOptional;
    bool SrcIsNilable = Src.IsNilable;
    bool SrcIsVariable = Src.Type->ActuallyIs<VariableParamHolder>();
    bool SrcIsNilableVariable = SrcIsNilable && SrcIsVariable;
    if(SrcIsNilableVariable)
    {
        SrcIsNilable = false;
        SrcIsVariable = false;
        SrcIsOpt = false;
    }
    bool SrcIsNormal = true;
    if(SrcIsOpt || SrcIsNilable || SrcIsVariable || SrcIsNilableVariable)
        SrcIsNormal = false;


    // Dest properties
    bool DstIsOpt = Dst.IsOptional;
    bool DstIsNilable = Dst.IsNilable;
    bool DstIsVariable = Dst.Type->ActuallyIs<VariableParamHolder>();
    bool DstIsNilableVariable = DstIsNilable && DstIsVariable;
    if(DstIsNilableVariable)
    {
        DstIsNilable = false;
        DstIsVariable = false;
        DstIsOpt = false;
    }
    bool DstIsNormal = true;
    if(DstIsOpt || DstIsNilable || DstIsVariable || DstIsNilableVariable)
        DstIsNormal = false;


/*                                  Dst
                    opt      nilable   var   nilable var  normal
 Src    opt             Y         Y        N        N          Y  
        nilable         N         Y        N        N          Y
        var             Y         N        Y        N          Y
        nilable var     Y         Y        Y        Y          Y
        normal          N         N        N        N          Y
*/
    if(SrcIsOpt)
    {
        if(DstIsOpt || DstIsNilable || DstIsNormal)
            return true;
        return false;
    }

    if(SrcIsNilable)
    {
        if(DstIsNilable || DstIsNormal)
            return true;
        return false;
    }

    if(SrcIsVariable)
    {
        if(DstIsOpt || DstIsVariable || DstIsNormal)
            return true;
        return false;
    }

    if(SrcIsNilableVariable)
    {
        return true;
    }

    if(SrcIsNormal)
    {
        if(DstIsNormal)
            return true;
        return false;
    }

    return false;
}

ETypeValidation FuncSigniture::ValidateConvert(SPtr<TypeDescBase> Target)
{
    if(EqualsTo(Target))
        return TCR_OK;

    ETypeValidation Result = TCR_OK;
    if(Target->ActuallyIs<FuncSigniture>())
    {
        SPtr<FuncSigniture> TargetFunc = Target->ActuallyAs<FuncSigniture>();
        if(HasThis != TargetFunc->HasThis)
            return TCR_NoWay;
        
        if(HasThis)
        {
            Result = MergeMulityValidation(Result,
                ThisType.Lock()->ValidateConvert(TargetFunc->ThisType.Lock()) );
            if(Result == TCR_NoWay)
                return TCR_NoWay;
        }

        for(int i = 0; i < TargetFunc->Returns.Count(); i++)
        {
            if(i >= Returns.Count())
            {
                // If target has more return values, they have to be nilable
                if(TargetFunc->Returns[i].IsNilable == false)
                    return TCR_NoWay;
            }
            else
            {
                // Nilable return values cannot convert to  non-nilables
                // All other cases are legal
                if(Returns[i].IsNilable == true && TargetFunc->Returns[i].IsNilable == false)
                    return TCR_NoWay;
                
                Result = MergeMulityValidation(Result,
                    Returns[i].Type->ValidateConvert(TargetFunc->Returns[i].Type.Lock()) );
                if(Result == TCR_NoWay)
                    return TCR_NoWay;
            }
        }

        int SrcIdx = 0;
        int TargetIdx = 0;
        FuncParamDesc* SrcParam = nullptr;
        FuncParamDesc* TargetParam = nullptr;
        while(true)
        {
            if(SrcIdx < Params.Count())
                SrcParam = &(Params[SrcIdx]);
            else
                SrcParam = nullptr;

            if(TargetIdx < TargetFunc->Params.Count())
                TargetParam = &(TargetFunc->Params[TargetIdx]);
            else
                TargetParam = nullptr;


            if(TargetParam == nullptr)
            {
                // Target finished all parameters
                // Validate that the Src parameters are all ignorable
                if(SrcParam == nullptr)
                    break;
                if(SrcParam->IsOptional || SrcParam->Type->ActuallyIs<VariableParamHolder>())
                    break;
               
                return TCR_NoWay;
            }
            else if(SrcParam == nullptr)
            {
                // if Src has variable param, this pointer will always stay at the last variable parameter, never being nullptr
                return TCR_NoWay;
            }

            if(MatchParamProperty(*SrcParam, *TargetParam) == false)
                return TCR_NoWay;
            
            SPtr<TypeDescBase> SrcRealType = SrcParam->Type.Lock();
            if(SrcRealType->ActuallyIs<VariableParamHolder>())
                SrcRealType = SrcRealType->ActuallyAs<VariableParamHolder>()->ParamType.Lock();

            SPtr<TypeDescBase> TargetRealType = TargetParam->Type.Lock();
            if(TargetRealType->ActuallyIs<VariableParamHolder>())
                TargetRealType = TargetRealType->ActuallyAs<VariableParamHolder>()->ParamType.Lock();

            // For function parameters, validate reversly from target parameter to src
            // So that the converted target can pass valid parameter to the source when calling
            Result = MergeMulityValidation(Result,
                TargetRealType->ValidateConvert(SrcRealType) );
            if(Result == TCR_NoWay)
                return TCR_NoWay;

            bool SrcIsVariable = SrcParam->Type->ActuallyIs<VariableParamHolder>();
            bool TargetIsVariable = TargetParam->Type->ActuallyIs<VariableParamHolder>();
            
            if(SrcIsVariable && TargetIsVariable)
            {
                break;
            }
            if(SrcIsVariable == false)
                SrcIdx++;
            if(TargetIsVariable == false)
                TargetIdx++;
            
        }

        return Result;
    }



    
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
            if(Params[i].IsNilable != TargetFunc->Params[i].IsNilable)
                return false;
            if(Params[i].Type.Lock()->EqualsTo(TargetFunc->Params[i].Type.Lock()) == false)
                return false;
        }

        for(int i = 0; i < Returns.Count(); i++)
        {
            if(Returns[i].IsNilable != TargetFunc->Returns[i].IsNilable)
                return false;
            if(Returns[i].Type.Lock()->EqualsTo(TargetFunc->Returns[i].Type.Lock()) == false)
                return false;
        }
        return true;
    }
    return false;
}


OLString FuncSigniture::ToString(bool IsNilable)
{
    OLString Ret;
    Ret.AppendF(T("function%s ("), (IsNilable?T("?"):T("")));
    bool BeginOptionalParam = false;
    for(int i = 0; i < Params.Count(); i++)
    {
        OLString TypeName = T("unknown_type");
        if(Params[i].Type != nullptr)
            TypeName = Params[i].Type->ToString(Params[i].IsNilable);

        if(Params[i].IsOptional)
        {
            BeginOptionalParam = true;
            Ret.Append(T("["));
        }

        OLString CurrDesc;
        if(Params[i].IsVariableParam)
        {
            CurrDesc = TypeName;
        }
        else
        {
            CurrDesc.Printf(T("_p%d as %s%s"), i, (Params[i].Flags & FPF_Const) ? T("const ") : T(""), TypeName.CStr());
        }
        if(i != Params.Count() - 1)
            Ret.AppendF(T("%s, "), CurrDesc.CStr());
        else
            Ret.AppendF(T("%s"), CurrDesc.CStr());
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
                TypeName = Returns[i].Type->ToString(Returns[i].IsNilable);

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

OperatorResult FuncSigniture::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<FuncSigniture>() || Target->IsAny() || Target->IsNil()))
    {
        return OperatorResult{IntrinsicType::CreateFromRaw(IT_bool), false};
    }

    return OperatorResult{nullptr, false};

     
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