/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/


#include "TypeInfoVisitor.h"
#include "SymbolTable.h"
#include "ClassType.h"
#include "FuncSigniture.h"
#include "AstInclude.h"
#include "AClassContructor.h"
#include "IntrinsicType.h"
#include "InterfaceType.h"
#include "EnumType.h"
#include "ArrayType.h"
#include "MapType.h"
#include "ConstructorType.h"
#include "VariableParamHolder.h"
namespace OL
{


TypeInfoVisitor::TypeInfoVisitor(SymbolTable& InSymboTable, CompileMsg& InCM) 
    : CM(InCM), ScopeVisitorBase(InSymboTable), ClassNestLevel(0)
{

}

OLString TypeInfoVisitor::MakeClassUniquName(SPtr<ClassType> Class)
{
    OLList<OLString> ScopeNames;
    SPtr<SymbolScope> Scope = CurrScope;
    while(Scope != nullptr)
    {
        SPtr<ABase> ScopeOwner = Scope->Owner.Lock();
        if(ScopeOwner->Is<AClass>())
            ScopeNames.Add(ScopeOwner->As<AClass>()->ClassName);
        else if(ScopeOwner->Is<AFuncDef>())
            ScopeNames.Add(ScopeOwner->As<AFuncDef>()->Name);
        else if(ScopeOwner->Is<ANormalMethod>())
            ScopeNames.Add(ScopeOwner->As<ANormalMethod>()->Name);
        else if(ScopeOwner->Is<AClassContructor>())
            ScopeNames.Add(T("ctor"));
        else if(ScopeOwner->Is<AFuncExpr>())
        {
            OLString Name;
            Name.Printf(T("funcexpr_%d_%d"), ScopeOwner->Line.Line, ScopeOwner->Line.Col);
            ScopeNames.Add(Name);
        }

        Scope = Scope->Parent.Lock();
    }

    OLString Ret;
    for(int i = ScopeNames.Count() - 1; i >= 0; i--)
    {
        if(i == ScopeNames.Count() - 1)
            Ret.Append(ScopeNames[i]);
        else
            Ret.AppendF(T("_%s"), ScopeNames[i].CStr());
    }
    return Ret;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AClass> Node)
{
    SPtr<ClassType> NewClass = new ClassType();
    NewClass->DeclNode = Node;
    NewClass->IsExternal = Node->IsExternal;
    SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node.Get());
    OL_ASSERT(Decl != nullptr);
    NewClass->ClassAttrib = Decl->Attrib;


    for(int i = 0; i < Node->BaseClasses.Count(); i++)
    {
        NewClass->AddUnresolvedBase(Node->BaseClasses[i]->As<ANamedType>()->TypeName);
    }
    NewClass->Name = Node->ClassName;
    NewClass->NestLevel = ClassNestLevel;
    ClassNestLevel++;

    CurrScope->TypeDefs.Add(NewClass);

    TypeStack.Add(NewClass);
    return ScopeVisitorBase::BeginVisit(Node);
}



EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AClass> Node)
{
    SPtr<ClassType> Class = TypeStack.Top().PtrAs<ClassType>();
    Class->SetInsideScope(CurrScope);
    Class->UniqueName = MakeClassUniquName(Class);
    Class->TryAddDefaultContructor();

    CurrScope->OwnerTypeDesc = TypeStack.Top();
    TypeStack.Pop();
    ClassNestLevel --;
    return ScopeVisitorBase::EndVisit(Node);  
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AInterface> Node)
{
    SPtr<InterfaceType> NewInterface = new InterfaceType();
    NewInterface->DeclNode = Node;
    for(int i = 0; i < Node->BaseInterfaces.Count(); i++)
    {
        NewInterface->AddUnresolvedBase(Node->BaseInterfaces[i]->As<ANamedType>()->TypeName);
    }
    NewInterface->Name = Node->InterfaceName;

    CurrScope->TypeDefs.Add(NewInterface);

    TypeStack.Add(NewInterface);
    return ScopeVisitorBase::BeginVisit(Node);
}



EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AInterface> Node)
{
    TypeStack.Top()->As<InterfaceType>()->SetInsideScope(CurrScope);
    CurrScope->OwnerTypeDesc = TypeStack.Top();
    TypeStack.Pop();
    return ScopeVisitorBase::EndVisit(Node);  
}


EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AFuncBody> Node)
{
    OL_ASSERT(TypeStack.Top()->Is<FuncSigniture>());

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AFuncBody> Node)
{
    return VS_Continue;
}


void TypeInfoVisitor::TryReceiveComplexSubType(SPtr<TypeDescBase> Owner, SPtr<TypeDescBase> SubType, SPtr<ATypeIdentity> Node)
{
    if(Owner->Is<FuncSigniture>())
    {
        SPtr<FuncSigniture> Func = Owner.PtrAs<FuncSigniture>();
        if(Func->IsReturnDecl(Node.Get()))
            Func->AddReturn(SubType, Node->Line);
    }
    else if(Owner->Is<ArrayType>())
    {
        SPtr<ArrayType> Arr = Owner.PtrAs<ArrayType>();
        if(Arr->IsElemTypeDecl(Node.Get()))
            Arr->SetElemType(SubType);
    }
    else if(Owner->Is<MapType>())
    {
        SPtr<MapType> Map = Owner.PtrAs<MapType>();
        if(Map->IsKeyTypeDecl(Node.Get()))
            Map->SetKeyType(SubType);
        else if(Map->IsValueTypeDecl(Node.Get()))
            Map->SetValueType(SubType);
    }
}
EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AArrayType> Node)
{
    SPtr<ArrayType> NewArray = new ArrayType();
    NewArray->DeclNode = Node;
    CurrScope->TypeDefs.Add(NewArray);
    TypeStack.Add(NewArray);

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AArrayType> Node)
{
    SPtr<ArrayType> Curr = TypeStack.Top().PtrAs<ArrayType>();
    TypeStack.Pop();

    if(TypeStack.Count() <= 0)
        return VS_Continue;

    SPtr<TypeDescBase> Top = TypeStack.Top();
    TryReceiveComplexSubType(Top, Curr, Node);

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AMapType> Node)
{
    SPtr<MapType> NewMap = new MapType();
    NewMap->DeclNode = Node;
    CurrScope->TypeDefs.Add(NewMap);
    TypeStack.Add(NewMap);

    return VS_Continue;
}
EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AMapType> Node)
{
    SPtr<MapType> Curr = TypeStack.Top().PtrAs<MapType>();
    TypeStack.Pop();

    if(Node->KeyType == nullptr)
    {
        Curr->SetKeyType(IntrinsicType::CreateFromRaw(IT_string));
    }

    if(TypeStack.Count() <= 0)
        return VS_Continue;

    SPtr<TypeDescBase> Top = TypeStack.Top();
    TryReceiveComplexSubType(Top, Curr, Node);


    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AFuncType> Node)
{
    SPtr<FuncSigniture> NewSig = new FuncSigniture();
    NewSig->DeclNode = Node;
    CurrScope->TypeDefs.Add(NewSig);
    TypeStack.Add(NewSig);
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AFuncType> Node)
{
    SPtr<FuncSigniture> Curr = TypeStack.Top().PtrAs<FuncSigniture>();
    TypeStack.Pop();

    if(TypeStack.Count() <= 0)
        return VS_Continue;

    SPtr<TypeDescBase> Top = TypeStack.Top();
    TryReceiveComplexSubType(Top, Curr, Node);

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::Visit(SPtr<ANamedType> Node)
{
    if(TypeStack.Count() <= 0)
        return VS_Continue;
    
    SPtr<TypeDescBase> Top = TypeStack.Top();
    if(Top->Is<FuncSigniture>())
    {
        SPtr<FuncSigniture> Func = Top.PtrAs<FuncSigniture>();
        if(Func->IsReturnDecl(Node.Get()))
            Func->AddReturn(Node->TypeName, Node->Line);
    }
    else if(Top->Is<ArrayType>())
    {
        SPtr<ArrayType> Arr = Top.PtrAs<ArrayType>();
        if(Arr->IsElemTypeDecl(Node.Get()))
            Arr->SetElemType(Node->TypeName);
    }
    else if(Top->Is<MapType>())
    {
        SPtr<MapType> Map = Top.PtrAs<MapType>();
        if(Map->IsKeyTypeDecl(Node.Get()))
            Map->SetKeyType(Node->TypeName);
        else if(Map->IsValueTypeDecl(Node.Get()))
            Map->SetValueType(Node->TypeName);
    }

    return VS_Continue;

}
EVisitStatus TypeInfoVisitor::Visit(SPtr<AIntrinsicType> Node)
{
    if(TypeStack.Count() <= 0)
        return VS_Continue;
    
    SPtr<TypeDescBase> Top = TypeStack.Top();
    if(Top->Is<FuncSigniture>())
    {
        SPtr<FuncSigniture> Func = Top.PtrAs<FuncSigniture>();
        if(Func->IsReturnDecl(Node.Get()))
            Func->AddReturn(Node->Type, Node->Line);
    }
    else if(Top->Is<ArrayType>())
    {
        SPtr<ArrayType> Arr = Top.PtrAs<ArrayType>();
        if(Arr->IsElemTypeDecl(Node.Get()))
            Arr->SetElemType(IntrinsicType::CreateFromRaw(Node->Type));
    }
    else if(Top->Is<MapType>())
    {
        SPtr<MapType> Map = Top.PtrAs<MapType>();
        if(Map->IsKeyTypeDecl(Node.Get()))
            Map->SetKeyType(IntrinsicType::CreateFromRaw(Node->Type));
        else if(Map->IsValueTypeDecl(Node.Get()))
            Map->SetValueType(IntrinsicType::CreateFromRaw(Node->Type));
    }

    return VS_Continue;
}


EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<ANormalMethod> Node)
{
    SPtr<FuncSigniture> NewSig = new FuncSigniture();
    NewSig->DeclNode = Node;
    CurrScope->TypeDefs.Add(NewSig);

    if(TypeStack.Top()->Is<ClassType>())
    {
        if (Node->Modifier->IsStatic == false)
            NewSig->SetThis(TypeStack.Top());
        TypeStack.Add(NewSig);
    }
    else if(TypeStack.Top()->Is<InterfaceType>())
    {
        NewSig->SetThis(TypeStack.Top());
        TypeStack.Add(NewSig);
    }
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<ANormalMethod> Node)
{

    SPtr<FuncSigniture> NewSig = TypeStack.Top().PtrAs<FuncSigniture>();
    TypeStack.Pop();

    if(TypeStack.Top()->Is<ClassType>())
    {
        SPtr<ClassType> Class = TypeStack.Top().PtrAs<ClassType>();
        
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node.Get());
        if (Decl != nullptr && Decl->DeclType == DT_Function)
        {
            Decl->ValueTypeDesc = NewSig;
        }

        Class->AddNormalMethod(Node, NewSig, Decl, CM);
    }
    else if(TypeStack.Top()->Is<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = TypeStack.Top().PtrAs<InterfaceType>();
        
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node.Get());
        if (Decl != nullptr && Decl->DeclType == DT_Function)
        {
            Decl->ValueTypeDesc = NewSig;
        }

        Interface->AddMethod(Node, NewSig, Decl, CM);
    }


    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AClassContructor> Node)
{
    
    SPtr<FuncSigniture> NewSig = new FuncSigniture();
    NewSig->DeclNode = Node;
    NewSig->SetThis(TypeStack.Top());

    CurrScope->TypeDefs.Add(NewSig);
    TypeStack.Add(NewSig);

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AClassContructor> Node)
{
    SPtr<FuncSigniture> NewSig = TypeStack.Top().PtrAs<FuncSigniture>();
    TypeStack.Pop();

    OL_ASSERT(TypeStack.Top()->Is<ClassType>());
    SPtr<ClassType> Class = TypeStack.Top().PtrAs<ClassType>();

    SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node.Get());
    if(Decl != nullptr && Decl->DeclType == DT_Function)
    {
        Decl->ValueTypeDesc = NewSig;
    }
    NewSig->SetCtorOwner(Class);
    Class->AddConstructor(Node, NewSig, Decl, CM);

    return VS_Continue;
}


EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AVarDecl> Node)
{
    if(TypeStack.Count() > 0)
    {
        SPtr<TypeDescBase> TopType = TypeStack.Top();
        if(TopType->Is<FuncSigniture>() 
            && TopType->As<FuncSigniture>()->IsParamDecl(Node.Get()))
        {
            return  VisitAsFuncParam(Node, TopType.PtrAs<FuncSigniture>());
        }
    }

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::VisitAsFuncParam(SPtr<AVarDecl> Node, SPtr<FuncSigniture> FuncSig)
{
    if(Node->VarType->Is<ANamedType>())
    {
        if(Node->IsVariableParam)
        {
            SPtr<VariableParamHolder> VariableParam = new VariableParamHolder(Node, Node->VarType->As<ANamedType>()->TypeName);
            CurrScope->TypeDefs.Add(VariableParam);
            FuncSig->AddParam(VariableParam, Node->IsConst, Node->IsVariableParam,  Node->IsOptionalParam, Node->Line);
        }   
        else
        {
            FuncSig->AddParam(Node->VarType->As<ANamedType>()->TypeName, Node->IsConst, Node->IsVariableParam, Node->IsOptionalParam, Node->Line);
        }
    }
    else if(Node->VarType->Is<AIntrinsicType>())
    {
        if(Node->IsVariableParam)
        {
            SPtr<VariableParamHolder> VariableParam = new VariableParamHolder(Node, IntrinsicType::CreateFromRaw(Node->VarType->As<AIntrinsicType>()->Type ));
            CurrScope->TypeDefs.Add(VariableParam);
            FuncSig->AddParam(VariableParam, Node->IsConst, Node->IsVariableParam,  Node->IsOptionalParam, Node->Line);
        }
        else
            FuncSig->AddParam(Node->VarType->As<AIntrinsicType>()->Type, Node->IsConst, Node->IsVariableParam, Node->IsOptionalParam, Node->Line);
    }
    else
    {
        SPtr<TypeDescBase> ParamType = CurrScope->FindTypeByNode(Node->VarType->As<AFuncType>(), false);
        if(ParamType == nullptr)
        {
            OL_ASSERT(0 && "Function param type should be ready here");
        }
        else
        {
            if(Node->IsVariableParam)
            {
                SPtr<VariableParamHolder> VariableParam = new VariableParamHolder(Node, ParamType);
                CurrScope->TypeDefs.Add(VariableParam);
                ParamType = VariableParam;
            }
            FuncSig->AddParam(ParamType, Node->IsConst, Node->IsVariableParam, Node->IsOptionalParam, Node->Line);
        }
    }



    return VS_Continue;
}


EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AFuncDef> Node)
{
    SPtr<FuncSigniture> NewSig = new FuncSigniture();
    NewSig->DeclNode = Node;
    
    SPtr<Declearation> DeclInfo = CurrScope->FindDecl(Node->Name, true, false);
    if(DeclInfo != nullptr && DeclInfo->DeclType == DT_Function)
    {
        DeclInfo->ValueTypeDesc = NewSig;
    }

    CurrScope->TypeDefs.Add(NewSig);

    TypeStack.Add(NewSig);
    return VS_Continue;

}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AFuncDef> Node)
{
    TypeStack.Pop();
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AFuncExpr> Node)
{
    SPtr<FuncSigniture> NewSig = new FuncSigniture();
    NewSig->DeclNode = Node;

    CurrScope->TypeDefs.Add(NewSig);

    TypeStack.Add(NewSig);
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AFuncExpr> Node)
{
    TypeStack.Pop();
    return VS_Continue;
}


EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AClassVar> Node)
{
    for(int i = 0; i < Node->Decls.Count(); i++)
    {
        SPtr<AVarDecl> DeclNode = Node->Decls[i];
        SPtr<AExpr> Init = nullptr;
        if(i < Node->Inits.Count())
            Init = Node->Inits[i];

        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(DeclNode.Get());
        OL_ASSERT(Decl != nullptr);
        TypeStack.Top()->As<ClassType>()->AddClassVar(DeclNode->VarName, Node->Modifier, DeclNode->VarType, Init, Decl, CM);
        
        // Set the decl to const if class member modified the decl to const
        if(Node->Modifier->IsConst)
            Decl->IsConst = true;
    }
    return VS_Continue;
}



EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AEnum> Node)
{
    SPtr<EnumType> NewEnum = new EnumType();
    NewEnum->DeclNode = Node;
    NewEnum->Name = Node->Name;

    CurrScope->TypeDefs.Add(NewEnum);

    TypeStack.Add(NewEnum);
    

    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AEnum> Node)
{
    TypeStack.Pop();
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AEnumItem> Node)
{
    OL_ASSERT(TypeStack.Top()->Is<EnumType>());

    SPtr<EnumType> Curr = TypeStack.Top().PtrAs<EnumType>();
    Curr->AddItem(Node);
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AEnumItem> Node)
{
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::BeginVisit(SPtr<AConstructor> Node)
{
    SPtr<ConstructorType> NewCtor = new ConstructorType();
    NewCtor->DeclNode = Node;
    CurrScope->TypeDefs.Add(NewCtor);
    TypeStack.Add(NewCtor);
    return VS_Continue;
}

EVisitStatus TypeInfoVisitor::EndVisit(SPtr<AConstructor> Node)
{
    TypeStack.Pop();
    return VS_Continue;
}



}