/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "SymbolWalker.h"
#include "AstInclude.h"
#include "ClassType.h"
#include "IntrinsicType.h"
#include "FuncSigniture.h"

namespace OL
{

SymbolWalker::SymbolWalker(SymbolTable& InSymbolTable, CompileMsg& InCM) 
    : CurrSymbolTable(InSymbolTable), CM(InCM)
{

}

EVisitStatus SymbolWalker::BeginVisit(SPtr<ARoot> Node)
{
    OwnerStack.Add(Node);

    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<ARoot> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<ABlock> Node)
{
    OL_ASSERT(WalkingStack.Count() > 0 && WalkingStack.Top()->Block.Get() == Node.Get());
    WalkingStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<ABlockStat> Node)
{
    OwnerStack.Add(Node);
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<ABlockStat> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AFuncDef> Node)
{
    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->ValueTypeNode = nullptr;
    NewDecl->DeclNode = Node;
    NewDecl->Name = Node->Name;
    NewDecl->DeclType = DT_Function;
    if(Node->Attrib != nullptr)
    {
        NewDecl->Attrib = new DeclearAttributes();
        NewDecl->Attrib->CreateAttributeSet(Node->Attrib->Items);
    }
    if(Node->Owner == nullptr)
    {
        NewDecl->IsExternal = false;
    }
    else
    {
        NewDecl->IsExternal = true;
    }
    NewDecl->IsConst = false;
    NewDecl->IsNilable = false;
    WalkingStack.Top()->AddDecl(NewDecl, CM);
    OwnerStack.Add(Node);
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<AFuncDef> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AFuncExpr> Node)
{
    OwnerStack.Add(Node);
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<AFuncExpr> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AMethod> Node)
{
    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->ValueTypeNode = nullptr;
    NewDecl->DeclNode = Node;
    if(Node->Is<ANormalMethod>())
    {
        if(Node->Attrib != nullptr)
        {
            NewDecl->Attrib = new DeclearAttributes();
            NewDecl->Attrib->CreateAttributeSet(Node->Attrib->Items);
        }
        NewDecl->Name = Node->As<ANormalMethod>()->Name;
    }
    else if(Node->Is<AClassContructor>())
        NewDecl->Name = Node->As<AClassContructor>()->Name;

    NewDecl->DeclType = DT_Function;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = Node->Modifier->IsConst;
    NewDecl->IsNilable = false;
    WalkingStack.Top()->AddDecl(NewDecl, CM);

    OwnerStack.Add(Node);
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<AMethod> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AClass> Node)
{
    for(int i = 0; i < Node->BaseClasses.Count(); i++)
    {
        if (Node->BaseClasses[i]->Is<ANamedType>())
        {
            SymbolScope::AddSymbolRef(Node, Node->BaseClasses[i]->As<ANamedType>()->TypeName, WalkingStack.Top(), true);

        }
        else
        {
            // Error:
        }
    }

    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->DeclNode = Node;
    NewDecl->DeclType = DT_Class;
    NewDecl->Name = Node->ClassName;
    NewDecl->ValueTypeNode = nullptr;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = false;
    NewDecl->IsNilable = false;
    if(Node->Attrib != nullptr)
    {
        NewDecl->Attrib = new DeclearAttributes();
        NewDecl->Attrib->CreateAttributeSet(Node->Attrib->Items);
    }

    WalkingStack.Top()->AddDecl(NewDecl, CM);



    OwnerStack.Add(Node);

    SPtr<SymbolScope> NewScope(new SymbolScope());
    NewScope->Owner = Node;
    NewScope->Block = nullptr;
    if(WalkingStack.Count() > 0)
    {
        NewScope->Parent = WalkingStack.Top();
    }

    NewScope->Type = ST_Class;
    
    NewScope->GenScopeName();
    CurrSymbolTable.AddSymbolScope(NewScope);
    WalkingStack.Add(NewScope); 

    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<AClass> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    //TypeDefStack.Pop();
    WalkingStack.Pop();

    return VS_Continue;
}


EVisitStatus SymbolWalker::BeginVisit(SPtr<AInterface> Node)
{
    for(int i = 0; i < Node->BaseInterfaces.Count(); i++)
    {
        if (Node->BaseInterfaces[i]->Is<ANamedType>())
        {
            SymbolScope::AddSymbolRef(Node, Node->BaseInterfaces[i]->As<ANamedType>()->TypeName, WalkingStack.Top(), true);
        }
        else
        {
            // Error:
        }
    }

    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->DeclNode = Node;
    NewDecl->DeclType = DT_Interface;
    NewDecl->Name = Node->InterfaceName;
    NewDecl->ValueTypeNode = nullptr;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = false;
    NewDecl->IsNilable = false;
    if(Node->Attrib != nullptr)
    {
        NewDecl->Attrib = new DeclearAttributes();
        NewDecl->Attrib->CreateAttributeSet(Node->Attrib->Items);
    }

    WalkingStack.Top()->AddDecl(NewDecl, CM);

    OwnerStack.Add(Node);

    SPtr<SymbolScope> NewScope(new SymbolScope());
    NewScope->Owner = Node;
    NewScope->Block = nullptr;
    if(WalkingStack.Count() > 0)
    {
        NewScope->Parent = WalkingStack.Top();
    }

    NewScope->Type = ST_Interface;
    
    NewScope->GenScopeName();
    CurrSymbolTable.AddSymbolScope(NewScope);
    WalkingStack.Add(NewScope); 

    return VS_Continue;
}
EVisitStatus SymbolWalker::EndVisit(SPtr<AInterface> Node)
{
    OL_ASSERT(OwnerStack.Top().Get() == Node->As<ABase>());
    OwnerStack.Pop();
    WalkingStack.Pop();

    return VS_Continue;
}


EVisitStatus SymbolWalker::BeginVisit(SPtr<AEnum> Node)
{
    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->DeclNode = Node;
    NewDecl->DeclType = DT_Enum;
    NewDecl->Name = Node->Name;
    NewDecl->ValueTypeNode = nullptr;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = false;
    NewDecl->IsNilable = false;
    WalkingStack.Top()->AddDecl(NewDecl, CM);
    return VS_Continue;
}

EVisitStatus SymbolWalker::EndVisit(SPtr<AEnum> Node)
{
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<ATypeCast> Node)
{
    // if (Node->TypeInfo->Is<ANamedType>())
    // {
    //     SymbolScope::AddSymbolRef(Node, Node->TypeInfo->As<ANamedType>()->TypeName, WalkingStack.Top(), true);
    // }
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AFuncBody> Node)
{
    // for(int i = 0; i < Node->ReturnType.Count(); i++)
    // {
    //     if (Node->ReturnType[i]->Is<ANamedType>())
    //     {
    //         SymbolScope::AddSymbolRef(Node, Node->ReturnType[i]->As<ANamedType>()->TypeName, WalkingStack.Top(), true);
    //     }
    // }

    return VS_Continue;
}

void SymbolWalker::AddVarDecl(SPtr<AVarDecl> DeclNode)
{
    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->ValueTypeNode = DeclNode->VarType;
    NewDecl->Name = DeclNode->VarName;
    NewDecl->DeclNode = DeclNode;
    NewDecl->DeclType = DT_Variant;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = DeclNode->IsConst;
    NewDecl->IsVariableParam = DeclNode->IsVariableParam;
    NewDecl->IsNilable = DeclNode->VarType->IsNilable;
    WalkingStack.Top()->AddDecl(NewDecl, CM);

}

EVisitStatus SymbolWalker::Visit(SPtr<ANamedType> Node)
{
    bool HasAddedRef = false;
    if(Node->Parent->Is<AClass>() && Node->Parent->As<AClass>()->IsBase(Node))
    {
        HasAddedRef = true;
    }

    if(Node->Parent->Is<AInterface>() && Node->Parent->As<AInterface>()->IsBase(Node))
    {
        HasAddedRef = true;
    }

    if(!HasAddedRef)
        SymbolScope::AddSymbolRef(Node->Parent.Lock(), Node->TypeName, WalkingStack.Top(), true);
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<ABlock> Node)
{
    SPtr<SymbolScope> NewScope(new SymbolScope());
    SPtr<ABase> Owner = OwnerStack.Top();
    NewScope->Owner = Owner;
    NewScope->Block = Node;
    if(WalkingStack.Count() > 0)
    {
        NewScope->Parent = WalkingStack.Top();
    }

    if(Owner->Is<ABlockStat>())
    {
        NewScope->Type = ST_Statements;
    }
    else if(Owner->Is<AFuncExpr>() || Owner->Is<AFuncDef>() || Owner->Is<AMethod>())
    {
        NewScope->Type = ST_Function;
    }
    else if(Owner->Is<ARoot>())
    {
        NewScope->Type = ST_Global;
    }
    else
    {
        OL_ASSERT("Unknown owner of ABlock");
    }
    NewScope->GenScopeName();

    CurrSymbolTable.AddSymbolScope(NewScope);
    WalkingStack.Add(NewScope);


    // Handle var declearation in 'for' statement title and function param
    
    if(Owner->Is<AForNum>())
    {
        AForNum* ForNum = Owner->As<AForNum>();
        AddVarDecl(ForNum->VarDecl);
    }
    else if(Owner->Is<AForList>())
    {
        AForList* ForList = Owner->As<AForList>();
        for(int i = 0; i < ForList->VarList.Count(); i++)
        {
            AddVarDecl(ForList->VarList[i]);
        }
    }
    else if(Node->Parent.Lock()->Is<AFuncBody>())
    {
        AFuncBody* FuncBody = Node->Parent.Lock()->As<AFuncBody>();
        for (int i = 0; i < FuncBody->Params.Count(); i++)
        {
            if(FuncBody->Params[i]->IsVariableParam)
                WalkingStack.Top()->HasVariableParam = true;
            AddVarDecl(FuncBody->Params[i]);
        }
        
    }
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AVarDecl> Node)
{
    // Ignore var declearation in 'for' statement title and function param
    // They are handled in later ABlock visit, as local vars of sub scope
    // For AFuncType, the declearation should be ignored because they are not a real symbol
    SPtr<ABase> NodeParent = Node->Parent.Lock();
    if(NodeParent->Is<AForList>() || NodeParent->Is<AForNum>() || NodeParent->Is<AFuncBody>() || NodeParent->Is<AFuncType>())
        return VS_Continue;

    AddVarDecl(Node);
    return VS_Continue;
}

EVisitStatus SymbolWalker::Visit(SPtr<AVarRef> Node)
{
    
    SymbolScope::AddSymbolRef(Node, Node->VarName, WalkingStack.Top(), false);
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<ADotMember> Node)
{
    Reference* Ref = new Reference();
    Ref->Name = Node->Field;
    Ref->RefNode = Node;
    Ref->Type = Ref_Deref;

    WalkingStack.Top()->Refs.Add(Ref);
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AColonCall> Node)
{
    Reference* Ref = new Reference();
    Ref->Name = Node->NameAfter;
    Ref->RefNode = Node;
    Ref->Type = Ref_Deref;

    WalkingStack.Top()->Refs.Add(Ref);
    return VS_Continue;
}

EVisitStatus SymbolWalker::BeginVisit(SPtr<AAlias> Node)
{
    SPtr<Declearation> NewDecl = new Declearation();
    NewDecl->ValueTypeNode = Node->TargetType;
    NewDecl->Name = Node->Name;
    NewDecl->DeclNode = Node;
    NewDecl->DeclType = DT_Alias;
    NewDecl->IsExternal = false;
    NewDecl->IsConst = false;
    NewDecl->IsVariableParam = false;
    NewDecl->IsNilable = false;

    WalkingStack.Top()->AddDecl(NewDecl, CM);
    return VS_Continue;
}

}