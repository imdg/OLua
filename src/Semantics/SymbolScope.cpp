/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "SymbolScope.h"
#include "AstCommon.h"
#include "AstInclude.h"
#include "AVarRef.h"
#include "Logger.h"
#include "TypeDescBase.h"
#include "ClassType.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
#include "ATypeIdentity.h"
#include "ANamedType.h"
#include "AIntrinsicType.h"
#include "CompileMsg.h"
#include "InterfaceType.h"
#include "EnumType.h"
#include "ImportedScopeGroup.h"
#include "TypeAlias.h"

namespace OL
{

STRUCT_RTTI_BEGIN(Declearation)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(ValueTypeNode, MF_External)
    RTTI_MEMBER(ValueTypeDesc, MF_External)
    RTTI_MEMBER(DeclNode, MF_External)
    RTTI_MEMBER(DeclType)
    RTTI_MEMBER(IsExternal)
    RTTI_MEMBER(IsConst)
    RTTI_MEMBER(IsVariableParam)
    RTTI_STRUCT_MEMBER_V(Attrib, DeclearAttributes)
STRUCT_RTTI_END(Declearation)

STRUCT_RTTI_BEGIN(Reference)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Type)
    RTTI_MEMBER(RefNode, MF_External)
    RTTI_MEMBER(IndirectRefNode, MF_External)
    RTTI_MEMBER(DeclScope, MF_External)
    RTTI_STRUCT_MEMBER(Decl, Declearation, MF_External)
    RTTI_STRUCT_MEMBER(RootRef, Reference, MF_External)
STRUCT_RTTI_END(Reference)


RTTI_BEGIN(SymbolScope)
    RTTI_MEMBER(ScopeName)
    RTTI_MEMBER(Type)
    RTTI_MEMBER(HasVariableParam)
    RTTI_MEMBER(Parent, MF_External)
    RTTI_MEMBER(Owner, MF_External)
    RTTI_MEMBER(OwnerTypeDesc, MF_External)
    RTTI_MEMBER(Block, MF_External)
    RTTI_MEMBER(Imports)
    RTTI_STRUCT_MEMBER(Decls, Declearation)
    RTTI_STRUCT_MEMBER(Refs, Reference)
    RTTI_MEMBER(TypeDefs)
RTTI_END(SymbolScope)

STRUCT_RTTI_BEGIN(DeclearAttributes)
    RTTI_STRUCT_MEMBER_V(Items, AttributeItem)
STRUCT_RTTI_END(DeclearAttributes)

void DeclearAttributes::CreateAttributeSet(OLList<AttributeItem>& Src)
{
    Items.Clear();
    for(int i = 0; i < Src.Count(); i++)
    {
        Items.Add(Src[i].Name, Src[i]);
    }
}


bool DeclearAttributes::HasItem(OLString Name)
{
    return Items.Exists(Name);
}
bool DeclearAttributes::HasItem(OLString Name, EIntrinsicType ExpectType)
{
    if(Items.Exists(Name) == false)
        return false;
    if(Items[Name].Type != ExpectType)
        return false;
    return true;
}

EIntrinsicType DeclearAttributes::GetType(OLString Name)
{
    return Items[Name].Type;
}

OLString DeclearAttributes::GetString(OLString Name)
{
    return Items[Name].StrVal;
}

int DeclearAttributes::GetInt(OLString Name)
{
    return Items[Name].IntVal;
}

float DeclearAttributes::GetFloat(OLString Name)
{
    return Items[Name].FltVal;
}
bool DeclearAttributes::GetBool(OLString Name)
{
    return Items[Name].BoolVal;
}



Reference::Reference() 
    : Type(Ref_Unresolved)
    , RefNode(nullptr)
    , IndirectRefNode(nullptr)
{

}

SymbolScope::SymbolScope()
    : Type(ST_Global)
    , Parent(nullptr)
    , Owner(nullptr)
    , Block(nullptr)
    , HasVariableParam(false)
{

}



SPtr<Declearation> SymbolScope::FindDecl(OLString Name, bool Vars, bool Types)
{
    for(int i = 0; i < Decls.Count(); i++)
    {
        if(Decls[i]->Name == Name)
        {
            if(Types)
            {
                if((Decls[i]->DeclType == DT_Class || Decls[i]->DeclType == DT_Interface || Decls[i]->DeclType == DT_Enum || Decls[i]->DeclType == DT_Alias ))
                    return Decls[i];
            }
            
            if(Vars)
            {
                if((Decls[i]->DeclType != DT_Class && Decls[i]->DeclType != DT_Interface && Decls[i]->DeclType != DT_Enum && Decls[i]->DeclType != DT_Alias))
                    return Decls[i];
            }
        }
    }
    return nullptr;
}

SPtr<Declearation> SymbolScope::FindDeclByNode(ABase* Node)
{
    for(int i = 0; i < Decls.Count(); i++)
    {
        if(Decls[i]->DeclNode.Get()->As<ABase>() == Node)
            return Decls[i];
    }
    return nullptr;
}

SPtr<Reference> SymbolScope::FindRefByNode(ABase* Node)
{
    for(int i = 0; i < Refs.Count(); i++)
    {
        if(Refs[i]->RefNode->As<ABase>() == Node)
        {
            return Refs[i];
        }
    }
    return nullptr;
}

SPtr<Declearation> SymbolScope::FindDeclInType(OLString Name, bool Vars, bool Types)
{
    if(OwnerTypeDesc == nullptr)
        return nullptr;
    
    SPtr<Declearation> Ret;
    if(OwnerTypeDesc->Is<ClassType>())
    {
        SPtr<ClassType> Class = OwnerTypeDesc.Lock().PtrAs<ClassType>();
        Ret = Class->InsideScope->FindDecl(Name, Vars, Types);
        if(Ret != nullptr)
            return Ret;

        for(int i = 0; i < Class->BaseTypes.Count(); i ++)
        {
            SPtr<SymbolScope> InsideScope;
            SPtr<TypeDescBase> Base = Class->BaseTypes[i].Lock();
            if(Base->Is<ClassType>())
                InsideScope = Base->As<ClassType>()->InsideScope.Lock();
            else if(Base->Is<InterfaceType>())
                InsideScope = Base->As<InterfaceType>()->InsideScope.Lock();

            if(InsideScope != nullptr)
            {
                Ret = InsideScope->FindDeclInType(Name, Vars, Types);
                if(Ret != nullptr)
                    return Ret;
            }

        }
    }
    else if(OwnerTypeDesc->Is<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = OwnerTypeDesc.Lock().PtrAs<InterfaceType>();
        Ret = Interface->InsideScope->FindDecl(Name, Vars, Types);
        if (Ret != nullptr)
            return Ret;

        for (int i = 0; i < Interface->BaseInterfaces.Count(); i++)
        {
            SPtr<SymbolScope> InsideScope;
            if (Interface->BaseInterfaces[i]->Is<InterfaceType>())
                InsideScope = Interface->BaseInterfaces[i].Lock()->InsideScope.Lock();

            if (InsideScope != nullptr)
            {
                Ret = InsideScope->FindDeclInType(Name, Vars, Types);
                if (Ret != nullptr)
                    return Ret;
            }
        }
    }

    return nullptr;
    
}



void SymbolScope::GenScopeName()
{
    OLString LocalName;
    if (Owner->Is<ABlockStat>())
        LocalName.AppendF(T("stat%d"), Block->Line.Line);
    else if (Owner->Is<AFuncDef>())
        LocalName.AppendF(T("%s%d"), Owner->As<AFuncDef>()->Name.CStr(), Block->Line.Line);
    else if (Owner->Is<ANormalMethod>())
        LocalName.AppendF(T("%s%d"), Owner->As<ANormalMethod>()->Name.CStr(), Block->Line.Line);
    else if (Owner->Is<AClassContructor>())
        LocalName.AppendF(T("ctor%d"), Block->Line.Line);
    else if (Owner->Is<AFuncExpr>())
        LocalName.AppendF(T("anonymous%d"), Block->Line.Line);
    else if (Owner->Is<AClass>())
        LocalName.AppendF(T("%s%d"), Owner->As<AClass>()->ClassName.CStr(), Owner->Line.Line);
    else if (Owner->Is<AInterface>())
        LocalName.AppendF(T("%s%d"), Owner->As<AInterface>()->InterfaceName.CStr(), Owner->Line.Line);
    if(Parent != nullptr)
        ScopeName.Printf(T("%s_%s"),Parent->ScopeName.CStr(), LocalName.CStr() );
    else
        ScopeName.Printf(T("S_%s"), LocalName.CStr() );
}

void    SymbolScope::AddDecl(SPtr<Declearation> NewDecl, CompileMsg& CM)
{

    for(int i = 0; i < Refs.Count(); i++)
    {
        if(Refs[i]->Type == Ref_UpVal && Refs[i]->Name == NewDecl->Name)
        {
            CM.Log(CMT_UseBeforeDecl, Refs[i]->RefNode.Lock()->Line, NewDecl->Name.CStr());
        }
    }
    for(int i = 0; i < Decls.Count(); i++)
    {
        if(Decls[i]->Name == NewDecl->Name)
        {
            CM.Log(CMT_NameConflict, NewDecl->DeclNode.Lock()->Line, Decls[i]->Name.CStr(), Decls[i]->DeclNode.Lock()->Line.Line);
        }
    }

    Decls.Add(NewDecl);
}

bool    SymbolScope::DeclIsType(SPtr<Declearation> Decl)
{
    if ((Decl->DeclType == DT_Class || Decl->DeclType == DT_Interface || Decl->DeclType == DT_Enum || Decl->DeclType == DT_Alias))
        return true;
    return false;
}

void SymbolScope::AddSymbolRef(SPtr<ABase> Node, OLString Name,  SPtr<SymbolScope> Scope, bool IsType)
{
    SPtr<SymbolScope> CurrScope = Scope;
    SPtr<ABase> LastIndirectRefNode = Node;
    OLList<Reference*> ToFinish;

    SPtr<Declearation> FoundDecl;
    SPtr<SymbolScope> FoundScope;
    // When passing ST_Class or ST_Interface, it should not search upwards because of unordered declearations in class
    // Leave it unresolved to resolve later
    if (CurrScope->Type == ST_Class || CurrScope->Type == ST_Interface)
    {
        Reference *NewRef = new Reference();
        NewRef->Name = Name;
        NewRef->RefNode = Node;
        NewRef->IndirectRefNode = LastIndirectRefNode;
        NewRef->Type = IsType ? Ref_UnresolvedType : Ref_Unresolved;
        CurrScope->Refs.Add(NewRef);
        return;
    }

    bool SearchVar = !IsType;
    bool SearchType = true; // Alway search type declearation because type name can be used to reference static member or enum item

    bool IsRealType = false;
    Reference* DontFinish = nullptr; // In the last level of a class. This shoud not be 'finished' as upval/uptype

    // Here CurrScope will never be a ST_Class/ST_Interface because the parent will be checked in advance
    while(true)
    {
        SPtr<SymbolScope> ParentScope = CurrScope->Parent.Lock();
        Reference* CurrNonLocalRef = nullptr;
        if(CurrScope->Type == ST_Statements || CurrScope->Type == ST_Function || CurrScope->Type == ST_Global)
        {
            SPtr<Declearation> Decl = CurrScope->FindDecl(Name, SearchVar, SearchType);
            if(Decl != nullptr)
            {
                IsRealType = DeclIsType(Decl);
                Reference *NewRef = new Reference();
                NewRef->Name = Name;
                NewRef->Decl = Decl;
                NewRef->DeclScope = CurrScope;
                NewRef->RefNode = Node;
                NewRef->IndirectRefNode = LastIndirectRefNode;
                NewRef->Type = IsRealType ? Ref_LocalType : Ref_Local;
                CurrScope->Refs.Add(NewRef);

                FoundDecl = Decl;
                FoundScope = CurrScope;
                 
                break;
            }
            else
            {
                Reference *NewRef = new Reference();
                NewRef->Name = Name;
                NewRef->RefNode = Node;
                NewRef->IndirectRefNode = LastIndirectRefNode;
                if(CurrScope->Type == ST_Global)
                    NewRef->Type = IsType ? Ref_UnresolvedType : Ref_Unresolved;
                else
                    NewRef->Type = IsType ? Ref_UpType : Ref_UpVal;
                CurrScope->Refs.Add(NewRef);

                ToFinish.Add(NewRef);
                CurrNonLocalRef = NewRef;
            }
        }

        // break if this is the final scope
        if(ParentScope == nullptr)
        {
            break;
        }

        // Did not resolved in current scope
        if(ParentScope->Type == ST_Class || ParentScope->Type == ST_Interface)
        {
            SPtr<Declearation> Decl = ParentScope->FindDecl(Name, SearchVar, SearchType);

            // Do not use FindDeclInType to find in base classes, because they are not processed probably

            if (Decl != nullptr)
            {
                // By search the parent, we know this is a member variable
                // Modify the Reference info as a member variable reference
                // 
                IsRealType = DeclIsType(Decl);
                //Reference *NewRef = new Reference();
                CurrNonLocalRef->Name = Name;
                CurrNonLocalRef->Decl = Decl;
                CurrNonLocalRef->DeclScope = ParentScope;
                CurrNonLocalRef->RefNode = Node;
                CurrNonLocalRef->IndirectRefNode = CurrScope->Owner;
                CurrNonLocalRef->Type = IsRealType ? Ref_MemberType : Ref_Member;
                //CurrScope->Refs.Add(NewRef);

                FoundDecl = Decl;
                FoundScope = ParentScope;
                DontFinish = CurrNonLocalRef;
            }
            else
            {
                if(CurrNonLocalRef)
                {
                    CurrNonLocalRef->Type = IsType ? Ref_UnresolvedType : Ref_Unresolved;
                    DontFinish = CurrNonLocalRef;
                }
            }
            break;
        }
        LastIndirectRefNode = CurrScope->Owner.Lock();
        CurrScope = ParentScope;
    }
    if(FoundDecl != nullptr)
    {
        for(int i = 0; i < ToFinish.Count(); i++)
        {
            if(ToFinish[i] != DontFinish) 
            {
                ToFinish[i]->Decl = FoundDecl;
                ToFinish[i]->DeclScope = FoundScope;
                if(ToFinish[i]->Type == Ref_UpVal && IsRealType == true)
                {
                    ToFinish[i]->Type = Ref_UpType;
                }
            }
        }
    }



}

bool SymbolScope::IsResolvedType(ERefType RefType)
{
    return RefType == Ref_LocalType || RefType == Ref_MemberType || RefType == Ref_UpType || RefType == Ref_ExternalType;
}

bool SymbolScope::IsResolvedVar(ERefType RefType)
{
    return RefType == Ref_Local || RefType == Ref_Member || RefType == Ref_UpVal || RefType == Ref_ExternalVal;
}


ERefType SymbolScope::RefTypeFromDecl(SPtr<Declearation> Decl, SPtr<SymbolScope> DeclScope, bool IsExternal)
{
    ERefType Ret = Ref_Unresolved;
    // Real type: like in AAA.bbb, AAA is considered as value initially, but can be a type in static member reference or enum item
    bool IsRealType = DeclIsType(Decl);
    if (DeclScope->Type == ST_Function || DeclScope->Type == ST_Statements)
    {
        OL_ASSERT(IsExternal == false);
        Ret = IsRealType ? Ref_LocalType : Ref_Local;
    }
    else if(DeclScope->Type == ST_Global)
    {
        // External symbol only exists in external global scope
        if(IsExternal)
            Ret = IsRealType ? Ref_ExternalType : Ref_ExternalVal;
        else
            Ret = IsRealType ? Ref_LocalType : Ref_Local;
    }
    else if (DeclScope->Type == ST_Class || DeclScope->Type == ST_Interface)
    {
         OL_ASSERT(IsExternal == false);
        Ret = IsRealType ? Ref_MemberType : Ref_Member;
    }
    return Ret;
}

void SymbolScope::FinishUnresolvedSymbol(OLList<SPtr<SymbolScope>>& InSymbolScopeList, CompileMsg& CM, ESymbolResolvePhase Phase)
{

    // Save informations: To update DeclScope and Decl at all References with specific RefNode
    struct DeclUpdateInfo
    {
        WPtr<ABase> RefNode;
        SPtr<SymbolScope> DeclScope;
        SPtr<Declearation> Decl;
        bool IsRealType;
    };
    OLList<DeclUpdateInfo> ToUpdate;

    struct ClearRefInfo
    {
        WPtr<ABase> RefNode;
    };

    OLList<ClearRefInfo> ToClear;

    for(int i = InSymbolScopeList.Count() - 1; i >= 0; i--)
    {
        SPtr<SymbolScope> CurrScope = InSymbolScopeList[i];

        for(int k = CurrScope->Refs.Count() - 1; k >= 0; k--)
        {
            for(int m = 0; m < ToClear.Count(); m++)
            {
                if(CurrScope->Refs[k]->RefNode.Get() == ToClear[m].RefNode.Get())
                {
                    CurrScope->Refs.RemoveAt(k);
                    break;
                }
            }
        }

        for(int k = 0; k < CurrScope->Refs.Count(); k++)
        {
            SPtr<Reference> Ref = CurrScope->Refs[k];
            if(Ref->Type != Ref_Unresolved && Ref->Type != Ref_UnresolvedType)
                continue;

            bool IsType = (Ref->Type == Ref_UnresolvedType) ? true : false;
            bool SearchVar = !IsType;
            bool SearchType = true; //  Alway search type declearation because type name can be used to reference static member or enum item


            bool Resolved = false;
            bool IsExternal = false;
            SPtr<Declearation> Decl = CurrScope->FindDecl(Ref->Name, SearchVar, SearchType);
            SPtr<SymbolScope> DeclScope = CurrScope;
            if(Decl == nullptr)
            {
                if(CurrScope->Imports != nullptr)
                {
                    ImportedDeclSearch Found = CurrScope->Imports->FindImportedDecl(Ref->Name, SearchVar, SearchType);
                    Decl = Found.Decl;
                    DeclScope = Found.DeclScope;
                    IsExternal = true;
                }
            }
            if (Decl != nullptr)
            {
                Ref->Type = RefTypeFromDecl(Decl, CurrScope, IsExternal);

                DeclUpdateInfo &UpdateInfo = ToUpdate.AddConstructed();
                UpdateInfo.Decl = Decl;
                UpdateInfo.DeclScope = DeclScope;
                UpdateInfo.RefNode = Ref->RefNode;
                UpdateInfo.IsRealType = DeclIsType(Decl);
                Resolved = true;
            }
            else
            {
                bool IsClassMember = false;
                SPtr<SymbolScope> Parent = CurrScope->Parent.Lock();
                if(Parent != nullptr && (Parent->Type == ST_Class || Parent->Type == ST_Interface))
                {
                    SPtr<Declearation> MemberDecl = Parent->FindDecl(Ref->Name, SearchVar, SearchType);
                    if(MemberDecl == nullptr)
                        MemberDecl = Parent->FindDeclInType(Ref->Name, SearchVar, SearchType);

                    if (MemberDecl != nullptr)
                    {
                        bool IsRealType = DeclIsType(MemberDecl);
                        Ref->Type = IsRealType ? Ref_MemberType : Ref_Member;
                        DeclUpdateInfo &UpdateInfo = ToUpdate.AddConstructed();
                        UpdateInfo.Decl = MemberDecl;
                        UpdateInfo.DeclScope = Parent;
                        UpdateInfo.RefNode = Ref->RefNode;
                        UpdateInfo.IsRealType = IsRealType;

                        // Make this mark to let upper scope to clear reference info if they have
                        // Because it is resolved in current level
                        // In muliti-phase resolvation, unresolved members (eg. member from base type) would be propogated to the root global scope
                        // in which case the upper level reference info need to be cleared if it is resolved as a member in sub scope class
                        ToClear.Add(ClearRefInfo{Ref->RefNode.Lock()});

                        IsClassMember = true;
                        Resolved = true;

                    }
                }

                // Only possible class member could be left unresolved in non-top scope
                if(IsClassMember == false && Parent != nullptr)
                {
                    Reference* UpLevelRef = new Reference();
                    UpLevelRef->Name = Ref->Name;
                    UpLevelRef->RefNode = Ref->RefNode;
                    UpLevelRef->IndirectRefNode = CurrScope->Owner;
                    UpLevelRef->Type = (Ref->Type == Ref_UnresolvedType) ? Ref_UnresolvedType : Ref_Unresolved;
                    Parent->Refs.Add(UpLevelRef);

                    // In a muliti-phase resolvation, only last step need to mark the middle level resolved as upval
                    // Otherwise leave them an unresolved status
                    if(Phase == SRP_GlobalVar || Phase == SRP_Standalone)
                        Ref->Type = (Ref->Type == Ref_UnresolvedType) ? Ref_UpType : Ref_UpVal;
                    Resolved = true;
                }

                if(Resolved == false)
                {
                    if(IsType == false && (Phase == SRP_GlobalVar || Phase == SRP_Standalone))
                    {
                        CM.Log(CMT_UnresolvedVar, Ref->RefNode->Line, Ref->Name.CStr());
                    }
                }
            }
        }
    }

    // Finish Decl and DeclScope, update every element of the upval/uptype chain
    for(int i = InSymbolScopeList.Count() - 1; i >= 0; i--)
    {
        SPtr<SymbolScope> CurrScope = InSymbolScopeList[i];
        for(int k = 0; k < CurrScope->Refs.Count(); k++)
        {
            SPtr<Reference> CurrRef = CurrScope->Refs[k];
            for(int m = 0; m < ToUpdate.Count(); m++)
            {
                if(CurrRef->RefNode == ToUpdate[m].RefNode)
                {
                    CurrRef->Decl = ToUpdate[m].Decl;
                    CurrRef->DeclScope = ToUpdate[m].DeclScope;
                    if(CurrRef->Type == Ref_UpVal && ToUpdate[m].IsRealType == true)
                        CurrRef->Type = Ref_UpType;
                }
            }
        }
    }

}

SPtr<TypeDescBase> SymbolScope::FindNamedType(OLString Name, bool Recursive)
{
    for(int i = 0; i < TypeDefs.Count(); i++)
    {
        SPtr<TypeDescBase> Curr = TypeDefs[i];
        if(Curr->IsValid() == false)
            continue;

        if(Curr->Is<ClassType>())
        {
            if(Curr->As<ClassType>()->Name == Name)
                return Curr;
        }
        else if(Curr->Is<InterfaceType>())
        {
            if(Curr->As<InterfaceType>()->Name == Name)
                return Curr;
        }
        else if(Curr->Is<EnumType>())
        {
            if(Curr->As<EnumType>()->Name == Name)
                return Curr;
        }
        else if(Curr->Is<TypeAlias>())
        {
            if(Curr->As<TypeAlias>()->Name == Name)
                return Curr;
        }
    }

    // External scope only exists in root scope
    // in which case the Parent must be nullptr

    if(Recursive && Parent != nullptr)
        return Parent->FindNamedType(Name, Recursive);

    if(Recursive && Imports != nullptr)
        return Imports->FindImportedNamedType(Name);

    return nullptr;

}


SPtr<TypeDescBase> SymbolScope::FindTypeByNode(ABase* Node, bool Recursive)
{
    if(Node->Is<ANamedType>())
        return FindNamedType(Node->As<ANamedType>()->TypeName, Recursive);

    if(Node->Is<AIntrinsicType>())
        return IntrinsicType::CreateFromRaw(Node->As<AIntrinsicType>()->Type);

    //Function signiture, array, map 
    for(int i = 0; i < TypeDefs.Count(); i++)
    {
        SPtr<TypeDescBase> Curr = TypeDefs[i];
        if(Curr->IsValid() == false)
            continue;
        if(Curr->DeclNode->As<ABase>() == Node)
            return Curr;
    }

    if(Recursive && Parent != nullptr)
        return Parent->FindTypeByNode(Node, Recursive);

    return nullptr;
}

void SymbolScope::ResolveTypes(CompileMsg& CM, ESymbolResolvePhase Phase)
{
    for(int i = 0; i < Decls.Count(); i++)
    {
        SPtr<Declearation> CurrDecl = Decls[i];
        if(CurrDecl->DeclType == DT_Variant)
        {
            if( CurrDecl->ValueTypeDesc != nullptr)
                continue;

            SPtr<ATypeIdentity>  ValueTypeNode = CurrDecl->ValueTypeNode.Lock();
            if(ValueTypeNode->Is<AIntrinsicType>())
            {
                SPtr<AIntrinsicType> IntrinsicNode = ValueTypeNode.PtrAs<AIntrinsicType>();
                CurrDecl->ValueTypeDesc = IntrinsicType::CreateFromRaw(IntrinsicNode->Type, IntrinsicNode->IsImplicitAny);
            }
            else if(ValueTypeNode->Is<ANamedType>())
            {
                CurrDecl->ValueTypeDesc = FindNamedType(ValueTypeNode->As<ANamedType>()->TypeName, true);

                if (CurrDecl->ValueTypeDesc == nullptr && (Phase == SRP_GlobalVar || Phase == SRP_Standalone))
                {
                    // Error: Done
                    CM.Log(CMT_NoType, CurrDecl->ValueTypeNode->Line,
                        CurrDecl->ValueTypeNode->As<ANamedType>()->TypeName.CStr() );
                    CurrDecl->ValueTypeDesc = IntrinsicType::CreateFromRaw(IT_any);
                }
            }
            else if(ValueTypeNode->Is<AFuncType>())
            {
                CurrDecl->ValueTypeDesc = FindTypeByNode(ValueTypeNode->As<AFuncType>(), true);
                if (CurrDecl->ValueTypeDesc == nullptr)
                {
                    // Error: Done
                    OL_ASSERT(0 && "No Signiture");
                }
            }
            else if(ValueTypeNode->Is<AArrayType>() || ValueTypeNode->Is<AMapType>())
            {
                CurrDecl->ValueTypeDesc = FindTypeByNode(ValueTypeNode->As<ABase>(), true);
                if (CurrDecl->ValueTypeDesc == nullptr)
                {
                    // Error: Done
                    OL_ASSERT(0 && "No Array or Map");
                }
            }

        }
    }

    for(int i = 0; i < TypeDefs.Count(); i++)
    {
        TypeDefs[i]->ResolveReferredType(this, CM, Phase);
    }
}


void SymbolScope::BindImport(SPtr<ImportedScopeGroup> InImportedScope)
{
    Imports = InImportedScope;
}



}