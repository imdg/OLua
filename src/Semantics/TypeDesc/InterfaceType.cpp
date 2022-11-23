/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "InterfaceType.h"
#include "ANormalMethod.h"
#include "AModifier.h"
#include "CompileMsg.h"
#include "SymbolScope.h"
#include "ClassType.h"
#include "IntrinsicType.h"
#include "SymbolScope.h"
namespace OL
{

STRUCT_RTTI_BEGIN(InterfaceMember)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Func, MF_External)
    RTTI_MEMBER(Flags)
    RTTI_STRUCT_MEMBER(MemberAttrib, DeclearAttributes)
STRUCT_RTTI_END(InterfaceMember)


RTTI_BEGIN_INHERITED(InterfaceType, TypeDescBase)
    RTTI_MEMBER(Name)
    RTTI_STRUCT_MEMBER(InterfaceAttrib, DeclearAttributes)
    RTTI_MEMBER(BaseInterfaces, MF_External)
    RTTI_STRUCT_MEMBER(UnresolvedBase, UnresolvedBaseTypeInfo)
    RTTI_MEMBER(InsideScope, MF_External)
    RTTI_STRUCT_MEMBER(Members, InterfaceMember)
RTTI_END(InterfaceType)


InterfaceType::~InterfaceType()
{
    
}

bool InterfaceType::ValidateMember(InterfaceMember& NewMember, CompileMsg& CM)
{
    if(NewMember.Name == T("new"))
    {
        CM.Log(CMT_ReservedNew, NewMember.Decl->DeclNode->Line);
        return false;
    }

    if(NewMember.Name == T("__add")
        || NewMember.Name == T("__sub")
        || NewMember.Name == T("__mul")
        || NewMember.Name == T("__div")
        || NewMember.Name == T("__concat")
        || NewMember.Name == T("__eq")
        || NewMember.Name == T("__le")
        || NewMember.Name == T("__lt")
        || NewMember.Name == T("__tostring"))
    {


        if((NewMember.Flags & CMF_Static) != 0 )
        {
            CM.Log(CMT_OpMustBeFunc, NewMember.Decl->DeclNode->Line, NewMember.Name.CStr());
            return false;
        }
        SPtr<FuncSigniture> Func = NewMember.Func.Lock();
        if(NewMember.Name == T("__tostring"))
        {
            if(Func->Params.Count() != 0)
            {
                CM.Log(CMT_OpParamCount, NewMember.Decl->DeclNode->Line, NewMember.Name.CStr(), 0);
                return false;
            }
        }
        else
        {
            if(Func->Params.Count() != 1)
            {
                CM.Log(CMT_OpParamCount, NewMember.Decl->DeclNode->Line, NewMember.Name.CStr(), 1);
                return false;
            }
        }

        if(Func->Returns.Count() != 1)
        {
            CM.Log(CMT_OpReturnCount, NewMember.Decl->DeclNode->Line, NewMember.Name.CStr());
            return false;
        }

        if(NewMember.Name == T("__eq")
            || NewMember.Name == T("__le")
            || NewMember.Name == T("__lt"))
        {
            if(Func->Returns[0].Type->IsBool() == false)
            {
                CM.Log(CMT_OpBoolRequired,  NewMember.Decl->DeclNode->Line, NewMember.Name.CStr());
                return false;
            }
        }
        if(NewMember.Name == T("__tostring"))
        {
            if(Func->Returns[0].Type->IsString() == false)
            {
                CM.Log(CMT_OpStringRequired,  NewMember.Decl->DeclNode->Line, NewMember.Name.CStr());
                return false;
            }
        }
    }
    return true;
}

void InterfaceType::AddMethod(SPtr<ANormalMethod> Node,  SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM)
{
    InterfaceMember& NewMember = Members.AddConstructed();
    
    // Interface is abstract, and only consider 'const' flag
    NewMember.Flags = CMF_Abstract;
    if(Node->Modifier->IsConst)
        NewMember.Flags |= CMF_Const;

    NewMember.Func = FuncSig;
    NewMember.Name = Node->Name;
    NewMember.Owner = SThis;
    NewMember.Decl = Decl;
    NewMember.MemberAttrib = Decl->Attrib;
    ValidateMember(NewMember, CM);
}


void InterfaceType::AddUnresolvedBase(OLString BaseName)
{
    UnresolvedBase.Add(UnresolvedBaseTypeInfo{BaseName, false});
}


void InterfaceType::SetInsideScope(SPtr<SymbolScope> Scope)
{
    InsideScope = Scope;
}

void InterfaceType::ResolveReferredType(SymbolScope* CurrScope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    for(int i = 0; i < UnresolvedBase.Count(); i++)
    {
        if(UnresolvedBase[i].Resolved == true)
            continue;
        SPtr<TypeDescBase> Found = CurrScope->FindNamedType(UnresolvedBase[i].Name, true);
        if(Found == nullptr)
        {
            // Error: Done
            if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
                CM.Log(CMT_NoBaseType, DeclNode->Line, UnresolvedBase[i].Name.CStr());
            continue;
        }
        else
        {
            UnresolvedBase[i].Resolved = true;
            if(Found->ActuallyIs<InterfaceType>())
            {
                BaseInterfaces.Add(Found);
            }
            else
            {
                CM.Log(CMT_InterfaceBaseTypeError, DeclNode->Line, UnresolvedBase[i].Name.CStr());
            }
        }
    }
}

bool InterfaceType::IsBaseType(SPtr<TypeDescBase> Base)
{
    for(int i = 0; i < BaseInterfaces.Count(); i++)
    {
        if(BaseInterfaces[i]->EqualsTo(Base))
            return true;
        
        SPtr<TypeDescBase> Curr = BaseInterfaces[i].Lock();
        if(Curr->ActuallyIs<InterfaceType>())
        {
            if(Curr->ActuallyAs<InterfaceType>()->IsBaseType(Base))
                return true;
        }
    }
    return false;
}

ETypeValidation InterfaceType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->ActuallyIs<IntrinsicType>())
    {
        SPtr<IntrinsicType> Intri = Target->ActuallyAs<IntrinsicType>();
        if(Intri->Type == IT_any)
            return TCR_OK;
    }

    if(Target->ActuallyIs<ClassType>())
    {
        SPtr<ClassType> Class = Target->ActuallyAs<ClassType>();

        if(Class->IsBaseType(SThis))
            return IsExplict ? TCR_OK : TCR_Unsafe;
    }
    else if(Target->ActuallyIs<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = Target->ActuallyAs<InterfaceType>();

        if(DeclNode == Interface->DeclNode)
            return TCR_OK;

        if(IsBaseType(Target))
            return TCR_OK;

        if(Interface->IsBaseType(SThis))
            return IsExplict ? TCR_OK : TCR_Unsafe;
    }

    return TCR_NoWay;
}

bool InterfaceType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<InterfaceType>() && DeclNode == Target->GetActualType()->DeclNode)
    {
        return true;
    }
    return false;
}

OLString InterfaceType::ToString()
{
    return Name;
}

InterfaceMember* InterfaceType::FindMember(OLString Name, bool IncludeBase)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Name == Name)
            return &(Members[i]);
    }

    if(IncludeBase)
    {
        for(int i = 0; i < BaseInterfaces.Count(); i++)
        {
            if(BaseInterfaces[i]->ActuallyIs<InterfaceType>())
            {
                InterfaceMember* Member = BaseInterfaces[i]->ActuallyAs<InterfaceType>()->FindMember(Name, true);
                if(Member != nullptr)
                    return Member;
            }
        }
    }
    return nullptr;
}

InterfaceMember* InterfaceType::FindMemberByDeclNode(SPtr<ABase> Node, bool IncludeBase)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Decl->DeclNode.Lock().PtrAs<ABase>() == Node)
            return &(Members[i]);
    }

    if(IncludeBase)
    {
        for(int i = 0; i < BaseInterfaces.Count(); i++)
        {
            if(BaseInterfaces[i]->ActuallyIs<InterfaceType>())
            {
                InterfaceMember* Member = BaseInterfaces[i]->ActuallyAs<InterfaceType>()->FindMemberByDeclNode(Node, true);
                if(Member != nullptr)
                    return Member;
            }
        }
    }
    return nullptr;
}

bool InterfaceType::IsNilable()
{
    return true;
}

SPtr<TypeDescBase> InterfaceType::AcceptBinOpOverride(EBinOp Op, SPtr<TypeDescBase> Target)
{
    OLString MetaName;
    switch (Op)
    {
    case BO_Add:   MetaName = T("__add");     break;
    case BO_Sub:   MetaName = T("__sub");     break;
    case BO_Mul:   MetaName = T("__mul");     break;
    case BO_Div:   MetaName = T("__div");     break;
    case BO_Cat:   MetaName = T("__concat");     break;
    case BO_Equal:      case BO_NotEqual:       MetaName = T("__eq");     break;
    case BO_Less:       case BO_Greater:        MetaName = T("__lt");     break;
    case BO_LessEqual:  case BO_GreaterEqual:   MetaName = T("__le");     break;

    default:
        return nullptr;
    };

    InterfaceMember* Found = FindMember(MetaName, true);
    SPtr<FuncSigniture> Func = nullptr;
    if(Found != nullptr)
        Func = Found->Func.Lock();

    if(Func == nullptr)
        return nullptr;

    if(Func->Params.Count() != 1)
        return nullptr;

    if(Target->ValidateConvert( Func->Params[0].Type.Lock(), false) == TCR_NoWay)
        return nullptr;

    if(Func->Returns.Count() > 0)
        return Func->Returns[0].Type.Lock();
    
    return nullptr;
}

SPtr<TypeDescBase> InterfaceType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<ClassType>() || Target->Is<InterfaceType>() || Target->IsAny() || Target->IsNil()))
    {
        return IntrinsicType::CreateFromRaw(IT_bool);
    }

    return nullptr;

     
}

SPtr<TypeDescBase> InterfaceType::AccecptUniOpOverride(EUniOp Op)
{
    if(Op != UO_Minus)
    {
        return nullptr;
    }

    InterfaceMember* Found = FindMember(T("__unm"), true);
    SPtr<FuncSigniture> Func = nullptr;
    if(Found != nullptr)
        Func = Found->Func.Lock();

    if(Func == nullptr)
        return nullptr;

    if(Func->Returns.Count() > 0)
        return Func->Returns[0].Type.Lock();
    return nullptr;
}

SPtr<TypeDescBase> InterfaceType::AcceptUniOp(EUniOp Op)
{
    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    return nullptr;
}



}