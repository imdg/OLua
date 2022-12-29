/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ClassType.h"
#include "AClassMember.h"
#include "AClassVar.h"
#include "ANormalMethod.h"
#include "AClassContructor.h"
#include "AVarDecl.h"
#include "AModifier.h"
#include "FuncSigniture.h"
#include "SymbolScope.h"
#include "IntrinsicType.h"
#include "AExpr.h"
#include "ATypeIdentity.h"
#include "AIntrinsicType.h"
#include "ANamedType.h"
#include "AFuncType.h"
#include "CompileMsg.h"
#include "InterfaceType.h"
#include "IntrinsicType.h"
#include "SymbolScope.h"
#include "BuiltinLib.h"

namespace OL
{

STRUCT_RTTI_BEGIN(ClassMemberDesc)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(DeclTypeDesc, MF_External)
    RTTI_MEMBER(RawTypeInfo, MF_External)
    RTTI_MEMBER(Flags)
    RTTI_MEMBER(IsNilable)
    RTTI_MEMBER(Init, MF_External)
    RTTI_STRUCT_MEMBER(Decl, Declearation,  MF_External)
    RTTI_STRUCT_MEMBER(MemberAttrib, DeclearAttributes)
STRUCT_RTTI_END(ClassMemberDesc)



RTTI_BEGIN_INHERITED(ClassType, TypeDescBase)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(UniqueName)
    RTTI_STRUCT_MEMBER(ClassAttrib, DeclearAttributes)
    RTTI_MEMBER(BaseTypes, MF_External)
    RTTI_MEMBER(IsExternal)
    RTTI_MEMBER(NestLevel)
    RTTI_MEMBER(IsReflection)
    RTTI_STRUCT_MEMBER(UnresolvedBase, UnresolvedBaseTypeInfo)
    RTTI_STRUCT_MEMBER(Members, ClassMemberDesc)
    RTTI_MEMBER(InsideScope, MF_External)
RTTI_END(ClassType)


ClassType::ClassType()
    : NestLevel(0), IsReflection(false), IsExternal(false)
{

}
ClassType::~ClassType()
{
    
}

uint ClassType::CreateFlag(SPtr<AModifier> Modifier)
{
    uint Flag = 0;
    if(Modifier->IsAbstract)    Flag |= CMF_Abstract;
    if(Modifier->IsConst)       Flag |= CMF_Const;
    if(Modifier->IsVirtual)     Flag |= CMF_Virtual;
    if(Modifier->IsStatic)      Flag |= CMF_Static;
    if(Modifier->IsOverride)    Flag |= CMF_Override;
    if(Modifier->Access == EAccessType::AT_Public)  Flag |= CMF_Public;
    if(Modifier->Access == EAccessType::AT_Protected)  Flag |= CMF_Protected;

    return Flag;
}

bool ClassType::ValidateMember(ClassMemberDesc& NewMember, CompileMsg& CM)
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
        || NewMember.Name == T("__mod")
        || NewMember.Name == T("__tostring")
        )
    {


        if(NewMember.DeclTypeDesc->ActuallyIs<FuncSigniture>() == false || (NewMember.Flags & CMF_Static) != 0 )
        {
            CM.Log(CMT_OpMustBeFunc, NewMember.Decl->DeclNode->Line, NewMember.Name.CStr());
            return false;
        }
        SPtr<FuncSigniture> Func = NewMember.DeclTypeDesc.Lock().PtrAs<FuncSigniture>();
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

void ClassType::AddClassVar(OLString Name, SPtr<AModifier> Modifier, SPtr<ATypeIdentity> RawType, SPtr<AExpr> Init, SPtr<Declearation> Decl, CompileMsg& CM)
{
    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = Name;
    NewMember.Flags = CreateFlag(Modifier) | ((Decl->IsConst) ? CMF_Const : 0); 
    NewMember.Type = Member_Variant;
    NewMember.RawTypeInfo = RawType;
    NewMember.Init = Init;
    if(IsExternal && Init != nullptr)
    {
        CM.Log(CMT_InitNotAllowOnExternal, Init->Line);
        NewMember.Init = nullptr;
    }
    NewMember.Owner = SThis;
    NewMember.Decl = Decl;
    NewMember.IsNilable = Decl->IsNilable;
    ValidateMember(NewMember, CM);
}

void ClassType::AddNormalMethod(SPtr<ANormalMethod> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM)
{
    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = Node->Name;
    NewMember.Flags = CreateFlag(Node->Modifier);
    NewMember.Type = Member_Function;
    NewMember.DeclTypeDesc = FuncSig;
    NewMember.Init = nullptr;
    NewMember.RawTypeInfo = nullptr;
    NewMember.Owner = SThis;
    NewMember.Decl = Decl;
    NewMember.MemberAttrib = Decl->Attrib;
    ValidateMember(NewMember, CM);
}

void ClassType::AddExtraNormalMethod(OLString Name, SPtr<FuncSigniture> FuncSig, bool IsConst, bool IsStatic, EAccessType Access)
{
    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = Name;
    NewMember.Flags = 0;
    NewMember.Type = Member_Function;
    NewMember.DeclTypeDesc = FuncSig;
    NewMember.Init = nullptr;
    NewMember.RawTypeInfo = nullptr;
    NewMember.Owner = SThis;
    
    if(IsConst)  NewMember.Flags |= CMF_Const;
    if(IsStatic)  NewMember.Flags |= CMF_Static;
    if(Access == EAccessType::AT_Public)  NewMember.Flags |= CMF_Public;
    if(Access == EAccessType::AT_Protected)  NewMember.Flags |= CMF_Protected;
}

void ClassType::AddConstructor(SPtr<AClassContructor> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM)
{
    if(IsExternal == true)
        return;
    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = Node->Name;
    NewMember.Flags = CreateFlag(Node->Modifier) | CMF_Constructor;
    NewMember.Type = Member_Function;
    NewMember.DeclTypeDesc = FuncSig;
    NewMember.Init = nullptr;
    NewMember.RawTypeInfo = nullptr;
    NewMember.Owner = SThis;
    NewMember.Decl = Decl;
    ValidateMember(NewMember, CM);
}

void ClassType::TryAddDefaultContructor()
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Name == T("new"))
        {
            return;
        }
    }

    SPtr<FuncSigniture> FuncSig = new FuncSigniture();
    InsideScope->TypeDefs.Add(FuncSig);
    FuncSig->SetCtorOwner(SThis);

    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = T("new");
    NewMember.Flags = CMF_Constructor | CMF_Public;
    NewMember.Type = Member_Function;
    NewMember.DeclTypeDesc = FuncSig;
    NewMember.Init = nullptr;
    NewMember.RawTypeInfo = nullptr;
    NewMember.Owner = SThis;
    NewMember.Decl = nullptr;
    
}

void ClassType::ResolveReferredType(SymbolScope* CurrScope, CompileMsg& CM, ESymbolResolvePhase Phase)
{
    bool HasBaseClass = false;
    for(int i = 0; i < UnresolvedBase.Count(); i++)
    {
        if(UnresolvedBase[i].Resolved)
            continue;
            
        SPtr<TypeDescBase> Found = CurrScope->FindNamedType(UnresolvedBase[i].Name, true);
        if(Found == nullptr)
        {
            if (Phase == SRP_GlobalVar || Phase == SRP_Standalone)
            {
                CM.Log(CMT_NoBaseType, DeclNode->Line, UnresolvedBase[i].Name.CStr());
            }
            continue;
        }
        else
        {
            UnresolvedBase[i].Resolved = true;
            if(Found->ActuallyIs<InterfaceType>())
                BaseTypes.Add(Found->ActuallyAs<InterfaceType>());
            else if(Found->ActuallyIs<ClassType>())
            {
                bool IsExternalBase = Found->ActuallyAs<ClassType>()->IsExternal;
                if((IsExternal && !IsExternalBase) || (!IsExternal && IsExternalBase))
                {
                    CM.Log(CMT_MixedInherit, DeclNode->Line);
                }
                else
                {
                    if(HasBaseClass == false)
                    {
                        HasBaseClass = true;
                        BaseTypes.Add(Found->GetActualType());
                    }
                    else
                        CM.Log(CMT_TooManyBaseClasses, DeclNode->Line);
                }
            }
            else
            {
                CM.Log(CMT_ClassBaseTypeError, DeclNode->Line, UnresolvedBase[i].Name.CStr());
            }
        }
    }

    for(int i = 0; i < Members.Count(); i++)
    {
        ClassMemberDesc& Curr = Members[i];
        if(Curr.Type == Member_Variant )
        {
            if(Curr.DeclTypeDesc == nullptr && Curr.RawTypeInfo != nullptr)
            {
                if( Curr.RawTypeInfo->Is<AIntrinsicType>())
                    Curr.DeclTypeDesc = IntrinsicType::CreateFromRaw(Curr.RawTypeInfo->As<AIntrinsicType>()->Type);
                else if (Curr.RawTypeInfo->Is<ANamedType>())
                    Curr.DeclTypeDesc = InsideScope->FindNamedType(Curr.RawTypeInfo->As<ANamedType>()->TypeName, true);
                else if (Curr.RawTypeInfo->Is<AFuncType>())
                    Curr.DeclTypeDesc = InsideScope->FindTypeByNode(Curr.RawTypeInfo->As<AFuncType>(), false);

            }
        }
    }
}

void ClassType::AddUnresolvedBase(OLString BaseClassName)
{
    UnresolvedBase.Add(UnresolvedBaseTypeInfo{BaseClassName, false});
}

void ClassType::SetInsideScope(SPtr<SymbolScope> Scope)
{
    InsideScope = Scope;
}

bool ClassType::IsBaseType(SPtr<TypeDescBase> Base)
{
    for(int i = 0; i < BaseTypes.Count(); i++)
    {
        if(BaseTypes[i].Get()->EqualsTo(Base))
            return true;
        
        SPtr<TypeDescBase> Curr = BaseTypes[i].Lock();
        if(Curr->ActuallyIs<ClassType>())
        {
            if(Curr->ActuallyAs<ClassType>()->IsBaseType(Base))
                return true;
        }
        else if(Curr->ActuallyIs<InterfaceType>())
        {
            if(Curr->ActuallyAs<InterfaceType>()->IsBaseType(Base))
                return true;
        }
    }
    return false;
}

ETypeValidation ClassType::ValidateConvert(SPtr<TypeDescBase> Target)
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
        if(DeclNode == Class->DeclNode)
            return TCR_OK;
        
        if(IsBaseType(Target))
            return TCR_OK;

        if(Class->IsBaseType(SThis))
            return TCR_Unsafe;
    }
    else if(Target->ActuallyIs<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = Target->ActuallyAs<InterfaceType>();
        if(IsBaseType(Target))
            return TCR_OK;

        if(Interface->IsBaseType(SThis))
            return TCR_Unsafe;
    }

    return TCR_NoWay;
}

bool ClassType::EqualsTo(SPtr<TypeDescBase> Target)
{
    if(Target->ActuallyIs<ClassType>())
    {
        SPtr<ClassType> Class = Target->ActuallyAs<ClassType>();
        if(DeclNode == Class->DeclNode)
            return true;
    }
    return false;
}

OLString ClassType::ToString(bool IsNilable)
{
    return Name + (IsNilable?T("?"):T(""));
}

FindMemberResult ClassType::FindMember(OLString Name, bool IncludeBase)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Name == Name)
        {
            FindMemberResult Result;
            Result.IsClassMember = true;
            Result.FromClass = &(Members[i]);
            return Result;
        }
    }
    if(IncludeBase)
    {
        for(int i = 0; i < BaseTypes.Count(); i++)
        {
            if(BaseTypes[i]->ActuallyIs<ClassType>())
            {
                FindMemberResult Result = BaseTypes[i]->ActuallyAs<ClassType>()->FindMember(Name, true);
                if(Result.FromClass != nullptr)
                {
                    return Result;
                }
            }
            else if(BaseTypes[i] ->ActuallyIs<InterfaceType>())
            {
                InterfaceMember* Member = BaseTypes[i]->ActuallyAs<InterfaceType>()->FindMember(Name, true);
                if(Member != nullptr)
                {
                    FindMemberResult Result;
                    Result.FromInterface = Member;
                    Result.IsClassMember = false;
                    return Result;
                }
            }
        }
    }

    FindMemberResult Result;
    Result.FromClass = nullptr;
    Result.IsClassMember = false;
    return Result;
}

FindMemberResult ClassType::FindMemberByDeclNode(SPtr<ABase> Node, bool IncludeBase)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Decl != nullptr && Members[i].Decl->DeclNode.Lock().PtrAs<ABase>() == Node)
        {
            FindMemberResult Result;
            Result.IsClassMember = true;
            Result.FromClass = &(Members[i]);
            return Result;
        }
    }
    if(IncludeBase)
    {
        for(int i = 0; i < BaseTypes.Count(); i++)
        {
            if(BaseTypes[i]->ActuallyIs<ClassType>())
            {
                FindMemberResult Result = BaseTypes[i]->ActuallyAs<ClassType>()->FindMemberByDeclNode(Node, true);
                if(Result.FromClass != nullptr)
                {
                    return Result;
                }
            }
            else if(BaseTypes[i] ->ActuallyIs<InterfaceType>())
            {
                InterfaceMember* Member = BaseTypes[i]->ActuallyAs<InterfaceType>()->FindMemberByDeclNode(Node, true);
                if(Member != nullptr)
                {
                    FindMemberResult Result;
                    Result.FromInterface = Member;
                    Result.IsClassMember = false;
                    return Result;
                }
            }
        }
    }

    FindMemberResult Result;
    Result.FromClass = nullptr;
    Result.IsClassMember = false;
    return Result;
}



SPtr<ClassType> ClassType::GetSuperClass()
{
    for(int i = 0; i < BaseTypes.Count(); i++)
    {
        if(BaseTypes[i]->Is<ClassType>())
            return BaseTypes[i].Lock();
    }
    return nullptr;
}

bool ClassType::IsMethod(SPtr<FuncSigniture> Func)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].DeclTypeDesc != nullptr
            && Members[i].DeclTypeDesc->ActuallyIs<FuncSigniture>()
            && Members[i].DeclTypeDesc->ActuallyAs<FuncSigniture>() == Func)
            return true;
    }

    return false;
}


SPtr<TypeDescBase> ClassType::AcceptBinOpOverride(EBinOp Op, SPtr<TypeDescBase> Target)
{
    OLString MetaName;
    switch (Op)
    {
    case BO_Add:   MetaName = T("__add");     break;
    case BO_Sub:   MetaName = T("__sub");     break;
    case BO_Mul:   MetaName = T("__mul");     break;
    case BO_Div:   MetaName = T("__div");     break;
    case BO_Cat:   MetaName = T("__concat");     break;
    case BO_Mod:   MetaName = T("__mod");     break;
    case BO_Equal:      case BO_NotEqual:       MetaName = T("__eq");     break;
    case BO_Less:       case BO_Greater:        MetaName = T("__lt");     break;
    case BO_LessEqual:  case BO_GreaterEqual:   MetaName = T("__le");     break;

    default:
        return nullptr;
    };

    FindMemberResult Found = FindMember(MetaName, true);
    SPtr<FuncSigniture> Func = nullptr;
    if(Found.IsClassMember && Found.FromClass != nullptr)
    {
        if(Found.FromClass->Type == Member_Function && (Found.FromClass->Flags & CMF_Static) == 0)
            Func = Found.FromClass->DeclTypeDesc->ActuallyAs<FuncSigniture>();
    }
    else if(!Found.IsClassMember && Found.FromInterface != nullptr)
    {
        if((Found.FromInterface->Flags & CMF_Static) == 0)
            Func = Found.FromInterface->Func.Lock();
    }

    if(Func == nullptr)
        return nullptr;

    if(Func->Params.Count() != 1)
        return nullptr;

    if(Target->ValidateConvert( Func->Params[0].Type.Lock()) == TCR_NoWay)
        return nullptr;

    if(Func->Returns.Count() > 0)
        return Func->Returns[0].Type.Lock();
    
    return nullptr;
}

 OperatorResult ClassType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable)
{
    SPtr<TypeDescBase> Override = AcceptBinOpOverride(Op, Target);
    if(Override != nullptr)
        return OperatorResult{Override, TargetNilable};

    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->ActuallyIs<ClassType>() || Target->ActuallyIs<InterfaceType>() || Target->IsAny() || Target->IsNil()))
    {
        return OperatorResult{IntrinsicType::CreateFromRaw(IT_bool), false};
    }

    return OperatorResult{nullptr, false};

     
}

SPtr<TypeDescBase> ClassType::AccecptUniOpOverride(EUniOp Op)
{
    if(Op != UO_Minus)
    {
        return nullptr;
    }

    FindMemberResult Found = FindMember(T("__unm"), true);
    SPtr<FuncSigniture> Func = nullptr;
    if(Found.IsClassMember && Found.FromClass != nullptr)
    {
        if(Found.FromClass->Type == Member_Function && (Found.FromClass->Flags & CMF_Static) == 0)
        {
            SPtr<FuncSigniture> Func = Found.FromClass->DeclTypeDesc->ActuallyAs<FuncSigniture>();
        }
    }
    else if(!Found.IsClassMember && Found.FromInterface != nullptr)
    {
        if((Found.FromInterface->Flags & CMF_Static) == 0)
            SPtr<FuncSigniture> Func = Found.FromInterface->Func.Lock();
    }

    if(Func == nullptr)
        return nullptr;

    if(Func->Returns.Count() > 0)
        return Func->Returns[0].Type.Lock();
    return nullptr;
}

SPtr<TypeDescBase> ClassType::AcceptUniOp(EUniOp Op)
{
    SPtr<TypeDescBase> Override = AccecptUniOpOverride(Op);
    if(Override != nullptr)
        return Override;

    if(Op == UO_Not)
        return IntrinsicType::CreateFromRaw(IT_bool);
    return nullptr;
}

void ClassType::FindVirtualFunc(OLList<FindMemberResult>& OutList)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if(Members[i].Type == Member_Function
            && (Members[i].Flags & (CMF_Virtual | CMF_Override) ))
        {
            bool Exists = false;

            for(int k = 0; k < OutList.Count(); k++)
            {
                if(OutList[k].FromClass->Name == Members[i].Name)
                {
                    Exists = true;
                    break;
                }
            }

            if(Exists == false)
            {
                FindMemberResult Item;
                Item.IsClassMember = true;
                Item.FromClass = &(Members[i]);
                OutList.Add(Item);
            }
        }
    }

    for(int i = 0; i < BaseTypes.Count(); i++)
    {
        if(BaseTypes[i]->ActuallyIs<ClassType>())
        {
            BaseTypes[i]->ActuallyAs<ClassType>()->FindVirtualFunc(OutList);
        }
    }
}

void ClassType::ForAllMembers(bool IncludeBase, OLFunc<bool(ClassMemberDesc&)> Callback)
{
    for(int i = 0; i < Members.Count(); i++)
    {
        if( false == Callback(Members[i]))
            return;
    }

    if(IncludeBase == false)
        return;

    for(int i = 0; i < BaseTypes.Count(); i++)
    {
        if(BaseTypes[i]->ActuallyIs<ClassType>())
        {
            BaseTypes[i]->ActuallyAs<ClassType>()->ForAllMembers(IncludeBase, Callback);
        }
    }
}

void ClassType::EnableReflection()
{
    if(IsReflection == true)
        return;

    IsReflection = true;
    
    // add static member "__type" and virtual function "__gettype()"
    
    ClassMemberDesc& StaticType = Members.AddConstructed();
    StaticType.Name = T("__type_info");
    StaticType.Flags = CMF_Const | CMF_Static | CMF_Public | CMF_Reserved;
    StaticType.Type = Member_Variant;
    StaticType.Owner = SThis;
    StaticType.IsNilable = false;
    StaticType.DeclTypeDesc = BuiltinLib::GetInst().GetTypeInfoClass();

    //InsideScope->Re

    SPtr<FuncSigniture> GetTypeFunc = new FuncSigniture();
    GetTypeFunc->AddReturn(BuiltinLib::GetInst().GetTypeInfoClass(), false, CodeLineInfo::Zero);
    GetTypeFunc->SetThis(SThis);
    InsideScope->TypeDefs.Add(GetTypeFunc);

    ClassMemberDesc& InstTypeGet = Members.AddConstructed();
    InstTypeGet.Name = T("__get_type_info");
    InstTypeGet.Flags = CMF_Const | CMF_Virtual | CMF_Public | CMF_Reserved;
    InstTypeGet.Type = Member_Function;
    InstTypeGet.Owner = SThis;
    InstTypeGet.IsNilable = false;
    InstTypeGet.DeclTypeDesc = GetTypeFunc;
}

}