#include "ArrayType.h"
#include "SymbolScope.h"
#include "CompileMsg.h"
#include "ATypeIdentity.h"
#include "AArrayType.h"
#include "IntrinsicType.h"

namespace OL
{

RTTI_BEGIN_INHERITED(ArrayType, TypeDescBase)
    RTTI_MEMBER(ElemType, MF_External)
    RTTI_MEMBER(UnresolvedElemType)
RTTI_END(ArrayType)

ArrayType::~ArrayType()
{
    
}

void ArrayType::ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    if(ElemType == nullptr && UnresolvedElemType != T(""))
    {
        SPtr<TypeDescBase> Found = Scope->FindNamedType(UnresolvedElemType, true);
        if(Found == nullptr)
        {
            if(Phase == SRP_GlobalVar || Phase == SRP_Standalone)
            {
                CM.Log(CMT_NoType, DeclNode->Line,  UnresolvedElemType.CStr());
                ElemType = IntrinsicType::CreateFromRaw(IT_any);
            }
            
        }
        else
        {
            ElemType = Found;
        }
    }
}

void ArrayType::SetElemType(SPtr<TypeDescBase> InElemType)
{
    ElemType = InElemType;
    UnresolvedElemType = T("");
}
void ArrayType::SetElemType(OLString UnresolvedName)
{
    ElemType = nullptr;
    UnresolvedElemType = UnresolvedName;
}

bool ArrayType::IsElemTypeDecl(ATypeIdentity* Node)
{
    if(DeclNode.Lock()->As<AArrayType>()->ElemType->As<ATypeIdentity>() == Node)
        return true;
    return false;
}

ETypeValidation ArrayType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(EqualsTo(Target.Get()))
        return TCR_OK;
    if(Target->Is<IntrinsicType>())
    {
        SPtr<IntrinsicType> Intri = Target.PtrAs<IntrinsicType>();
        if(Intri->Type == IT_any)
            return TCR_OK;
    }
    return TCR_NoWay;
}

bool ArrayType::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<ArrayType>() == false)
        return false;

    if(ElemType.Lock()->EqualsTo(Target->As<ArrayType>()->ElemType.Get()))
    {
        return true;
    }

    return false;

}

OLString ArrayType::ToString()
{
    OLString Ret;
    Ret.AppendF(T("%s[]"), ElemType->ToString().CStr());
    return Ret;
}

bool ArrayType::IsNilable()
{
    return true;
}

SPtr<TypeDescBase> ArrayType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<ArrayType>() || Target->IsAny() || Target->IsNil()))
    {
        return IntrinsicType::CreateFromRaw(IT_bool);
    }

    return nullptr;

     
}

SPtr<TypeDescBase> ArrayType::AcceptUniOp(EUniOp Op)
{
    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    if(Op == UO_Length)
        return IntrinsicType::CreateFromRaw(IT_int);

    return nullptr;
}

}