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

namespace OL
{

STRUCT_RTTI_BEGIN(ClassMemberDesc)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(DeclTypeDesc, MF_External)
    RTTI_MEMBER(RawTypeInfo, MF_External)
    RTTI_MEMBER(Flags)
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
    RTTI_STRUCT_MEMBER(UnresolvedBase, UnresolvedBaseTypeInfo)
    RTTI_STRUCT_MEMBER(Members, ClassMemberDesc)
    RTTI_MEMBER(InsideScope, MF_External)
RTTI_END(ClassType)


ClassType::ClassType()
    : NestLevel(0)
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
        || NewMember.Name == T("__tostring")
        )
    {


        if(NewMember.DeclTypeDesc->Is<FuncSigniture>() == false || (NewMember.Flags & CMF_Static) != 0 )
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

void ClassType::AddConstructor(SPtr<AClassContructor> Node, SPtr<FuncSigniture> FuncSig, SPtr<Declearation> Decl, CompileMsg& CM)
{
    if(IsExternal == true)
        return;
    ClassMemberDesc& NewMember = Members.AddConstructed();
    NewMember.Name = Node->Name;
    NewMember.Flags = CMF_Constructor;
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
    NewMember.Flags = CMF_Constructor;
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
            if(Found->Is<InterfaceType>())
                BaseTypes.Add(Found);
            else if(Found->Is<ClassType>())
            {
                bool IsExternalBase = Found->As<ClassType>()->IsExternal;
                if((IsExternal && !IsExternalBase) || (!IsExternal && IsExternalBase))
                {
                    CM.Log(CMT_MixedInherit, DeclNode->Line);
                }
                else
                {
                    if(HasBaseClass == false)
                    {
                        HasBaseClass = true;
                        BaseTypes.Add(Found);
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
                {
                    Curr.DeclTypeDesc = InsideScope->FindTypeByNode(Curr.RawTypeInfo->As<AFuncType>(), false);
                }
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

bool ClassType::IsBaseType(TypeDescBase* Base)
{
    for(int i = 0; i < BaseTypes.Count(); i++)
    {
        if(BaseTypes[i].Get()->EqualsTo(Base))
            return true;
        
        SPtr<TypeDescBase> Curr = BaseTypes[i].Lock();
        if(Curr->Is<ClassType>())
        {
            if(Curr->As<ClassType>()->IsBaseType(Base))
                return true;
        }
        else if(Curr->Is<InterfaceType>())
        {
            if(Curr->As<InterfaceType>()->IsBaseType(Base))
                return true;
        }
    }
    return false;
}

ETypeValidation ClassType::ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict)
{
    if(Target->Is<IntrinsicType>())
    {
        SPtr<IntrinsicType> Intri = Target.PtrAs<IntrinsicType>();
        if(Intri->Type == IT_any)
            return TCR_OK;
    }

    if(Target->Is<ClassType>())
    {
        SPtr<ClassType> Class = Target.PtrAs<ClassType>();
        if(DeclNode == Class->DeclNode)
            return TCR_OK;
        
        if(IsBaseType(Target.Get()))
            return TCR_OK;

        if(Class->IsBaseType(this))
            return IsExplict ? TCR_OK : TCR_Unsafe;
    }
    else if(Target->Is<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = Target.PtrAs<InterfaceType>();
        if(IsBaseType(Target.Get()))
            return TCR_OK;

        if(Interface->IsBaseType(this))
            return IsExplict ? TCR_OK : TCR_Unsafe;
    }

    return TCR_NoWay;
}

bool ClassType::EqualsTo(TypeDescBase* Target)
{
    if(Target->Is<ClassType>())
    {
        ClassType* Class = Target->As<ClassType>();
        if(DeclNode == Class->DeclNode)
            return true;
    }
    return false;
}

OLString ClassType::ToString()
{
    return Name;
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
            if(BaseTypes[i]->Is<ClassType>())
            {
                FindMemberResult Result = BaseTypes[i]->As<ClassType>()->FindMember(Name, true);
                if(Result.FromClass != nullptr)
                {
                    return Result;
                }
            }
            else if(BaseTypes[i] ->Is<InterfaceType>())
            {
                InterfaceMember* Member = BaseTypes[i]->As<InterfaceType>()->FindMember(Name, true);
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
            if(BaseTypes[i]->Is<ClassType>())
            {
                FindMemberResult Result = BaseTypes[i]->As<ClassType>()->FindMemberByDeclNode(Node, true);
                if(Result.FromClass != nullptr)
                {
                    return Result;
                }
            }
            else if(BaseTypes[i] ->Is<InterfaceType>())
            {
                InterfaceMember* Member = BaseTypes[i]->As<InterfaceType>()->FindMemberByDeclNode(Node, true);
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
            && Members[i].DeclTypeDesc->Is<FuncSigniture>()
            && Members[i].DeclTypeDesc.Lock().PtrAs<FuncSigniture>() == Func)
            return true;
    }

    return false;
}

bool ClassType::IsNilable()
{
    return true;
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
            Func = Found.FromClass->DeclTypeDesc.Lock().PtrAs<FuncSigniture>();
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

    if(Target->ValidateConvert( Func->Params[0].Type.Lock(), false) == TCR_NoWay)
        return nullptr;

    if(Func->Returns.Count() > 0)
        return Func->Returns[0].Type.Lock();
    
    return nullptr;
}

SPtr<TypeDescBase> ClassType::AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target)
{
    SPtr<TypeDescBase> Override = AcceptBinOpOverride(Op, Target);
    if(Override != nullptr)
        return Override;

    if( (Op == BO_And || Op == BO_Or || Op == BO_Equal || Op == BO_NotEqual)
        && (Target->Is<ClassType>() || Target->Is<InterfaceType>() || Target->IsAny() || Target->IsNil()))
    {
        return IntrinsicType::CreateFromRaw(IT_bool);
    }

    return nullptr;

     
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
            SPtr<FuncSigniture> Func = Found.FromClass->DeclTypeDesc.Lock().PtrAs<FuncSigniture>();
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
        if(BaseTypes[i].Lock()->Is<ClassType>())
        {
            BaseTypes[i].Lock().PtrAs<ClassType>()->FindVirtualFunc(OutList);
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
        if(BaseTypes[i].Lock()->Is<ClassType>())
        {
            BaseTypes[i].Lock().PtrAs<ClassType>()->ForAllMembers(IncludeBase, Callback);
        }
    }
}

}