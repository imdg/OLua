/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "LuaPlainInterp.h"
#include "AstInclude.h"
#include "TypeDescBase.h"
#include "SymbolScope.h"
#include "ClassType.h"
#include "EnumType.h"
#include "InterfaceType.h"
#include "Logger.h"
#include "LPClassHelper.h"
#include "ArrayType.h"
#include "MapType.h"
#include "ConstructorType.h"
#include "TupleType.h"
#include "LPClassLib.h"
namespace OL
{
#define ASSERT_CHILD_NUM(index, count) OL_ASSERT((count) == (ContentStack.Count() - 1 - (index)))

#define SELF_OBJ_NAME T("__self_obj_")

LuaPlainInterp::NodeGen::NodeGen(SPtr<ABase> InNode, SPtr<TextParagraph> InCurrText)
    : Node(InNode), CurrText(InCurrText), ExtraText(nullptr)
{

}

LuaPlainInterp::LuaPlainInterp(SymbolTable& InSymbolTable, TextBuilder& InOutText, LPClassLib& InClassLib) 
    : ScopeVisitorBase(InSymbolTable), OutText(InOutText), ClassLib(InClassLib)
{
    SelfNameStack.Add(T(""));
}



// OLString LuaPlainInterp::MakeMemberName(SPtr<ClassType> Class, OLString MemberName, OLString Suffix)
// {
//     OLString Ret;
//     Ret.Printf(T("__CM_%s_%s_%s"), Class->UniqueName.CStr(), MemberName.CStr(), Suffix.CStr());
//     return Ret;
// }

// OLString LuaPlainInterp::MakeConstructorName(SPtr<ClassType> Class, OLString MemberName, OLString Suffix)
// {
//     OLString Ret;
//     Ret.Printf(T("__CM_%s_ctor_%s_%s"), Class->UniqueName.CStr(), MemberName.CStr(), Suffix.CStr());
//     return Ret;
// }

// OLString LuaPlainInterp::MakeVTableName(SPtr<ClassType> Class, OLString Suffix)
// {
//     OLString Ret;
//     Ret.Printf(T("__CV_%s_%s"), Class->UniqueName.CStr(),  Suffix.CStr());
//     return Ret;
// }

// OLString LuaPlainInterp::MakeStaticTableName(SPtr<ClassType> Class, OLString Suffix)
// {
//     OLString Ret;
//     Ret.Printf(T("__CS_%s_%s"), Class->UniqueName.CStr(),  Suffix.CStr());
//     return Ret;
// }

// OLString LuaPlainInterp::MakeSelfName(SPtr<ClassType> Class)
// {
//     OLString Ret;
//     Ret.Printf(T("%s%d"), SELF_OBJ_NAME, Class->NestLevel );
//     return Ret;
// }

SPtr<TextParagraph> LuaPlainInterp::FromTop(int CurrIndex, int TopIndex)
{
    return ContentStack[CurrIndex + TopIndex + 1].CurrText;
}



LuaPlainInterp::NodeGen& LuaPlainInterp::NodeFromTop(int CurrIndex, int TopIndex)
{
    return ContentStack[CurrIndex + TopIndex + 1];
}

int LuaPlainInterp::StackCount(int CurrIndex)
{
    return ContentStack.Count() - CurrIndex - 1;
}

LuaPlainInterp::NodeGen* LuaPlainInterp::FindStack(int CurrIndex, SPtr<ABase> Node)
{
    for(int i = CurrIndex + 1; i < ContentStack.Count(); i++)
    {
        if(ContentStack[i].Node == Node)
            return &(ContentStack[i]);
    }
    return nullptr;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ARoot> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.Root));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus LuaPlainInterp::EndVisit(SPtr<ARoot> Node)
{
    int Index = IndexStack.PickPop();
    OL_ASSERT(ContentStack[Index].CurrText == OutText.Root);

    int Count = StackCount(Index);
    for(int i = 0; i < Count; i++)
    {
        ContentStack[Index].CurrText->Merge(FromTop(Index, i));
    }
    ContentStack.PopTo(Index);
    return VS_Continue;
}

void LuaPlainInterp::MergeStaticBlock()
{
    OutText.Root->NewLine().Append(T("----------------- Standalone static block---------------------")).NewLine().IndentInc();
    for(int i = 0; i < PendingStaticTable.Count(); i++)
    {
        OutText.Root->Merge(PendingStaticTable[i].StaticTableText);
    }
    OutText.Root->IndentDec();
}

SPtr<TextParagraph> LuaPlainInterp::MakeStandaloneStaticBlock()
{
    SPtr<TextParagraph> StaticBlock = OutText.NewParagraph();
    StaticBlock->NewLine().IndentInc();
        for(int i = 0; i < PendingStaticTable.Count(); i++)
    {
        StaticBlock->Merge(PendingStaticTable[i].StaticTableText);
    }
    
    StaticBlock->IndentDec().NewLine();

    return StaticBlock;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ABlock> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return ScopeVisitorBase::BeginVisit(Node);
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ABlock> Node)
{
    int Index = IndexStack.PickPop();
    
    // comment this. some stats does not generate any codes. like: interface, enum
    //ASSERT_CHILD_NUM(Index, Node->Stats.Count());

    //Is constructor
    bool IsClassConstructor = false;
    if(Node->Parent->Is<AFuncBody>())
    {
        if(Node->Parent->Parent->Is<AClassContructor>())
            IsClassConstructor = true;
    }


    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;
    CurrText->IndentInc();

    // if(IsClassConstructor)
    // {
    //     CurrText->Hold(T("__hidden_define")).Hold(T("__hidden_ctor_call"));
    // }

    int Count = StackCount(Index);
    for(int i = 0; i < Count; i++)
    {
        CurrText->Merge(FromTop(Index, i));
    }
    
    if(IsClassConstructor)
    {
        CurrText->Indent().AppendF(T("return %s"), CurrSelfName.CStr()).NewLine();
    }
    CurrText->IndentDec();

    ContentStack.PopTo(Index);
    return ScopeVisitorBase::EndVisit(Node);
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AParentheses> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}



EVisitStatus LuaPlainInterp::EndVisit(SPtr<AParentheses> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    ContentStack[Index].CurrText
        ->Append(T("("))
        .Merge(ContentStack.Top().CurrText)
        .Append(T(")"));

    ContentStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<ASelf> Node)
{
    SPtr<TextParagraph> Text = OutText.NewParagraph();
    Text->Append(CurrSelfName);
    ContentStack.Add(NodeGen(Node, Text));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<ASuper> Node)
{
    SPtr<TextParagraph> Text = OutText.NewParagraph();
    Text->Append(CurrSelfName);
    ContentStack.Add(NodeGen(Node, Text));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<AConstExpr> Node)
{
    OLString tmp;
    SPtr<TextParagraph> Text = OutText.NewParagraph();
    switch(Node->Type)
    {
    case IT_int:
        Text->AppendF(T("%d"), Node->IntVal);
        break;
    case IT_float:
        Text->AppendF(T("%f"), Node->FltVal);
        break;   
    case IT_string:
        // to do: string modifiers
        Text->AppendF(T("\"%s\""), Node->StrVal.CStr());

        break;   
    case IT_bool:
        Text->AppendF(T("%s"), Node->BoolVal ? T("true") : T("false"));
        break;   
    case IT_nil:
        Text->AppendF(T("nil"));
        break;    
    }

    ContentStack.Add(NodeGen(Node, Text));

    return VS_Continue;
}


EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AConstructor> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AConstructor> Node)
{
    int Index = IndexStack.PickPop();
    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;

    Text->Append(T("{ "));
    if(Node->HasKey)
    {
        ASSERT_CHILD_NUM(Index, Node->Elems.Count() * 2);
        for(int i = 0; i < Node->Elems.Count(); i++)
        {
            Text->Append(T("["))
                .Merge(FromTop(Index, i * 2))
                .Append(T("] = "))
                .Merge(FromTop(Index, i * 2 + 1));
            if(i != Node->Elems.Count() - 1)
                Text->Append(T(", "));
        }

    }
    else
    {
        ASSERT_CHILD_NUM(Index, Node->Elems.Count());
        for(int i = 0; i < Node->Elems.Count(); i++)
        {
            Text->Merge(FromTop(Index, i));
            if(i != Node->Elems.Count() - 1)
                Text->Append(T(", "));
        }
    }

    Text->Append(T(" }"));

    ContentStack.PopTo(Index);

    return VS_Continue;
}


EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AColonCall> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AColonCall> Node)
{
    int Index = IndexStack.PickPop();
    //ASSERT_CHILD_NUM(Index, Node->Params.Count() + 1);

    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;


    SPtr<AExpr> Target = Node->Func;
    
    bool IsVirtual = false;
    bool IsRawStyle = false;

    if(Target->ExprType.Lock()->ActuallyIs<ClassType>())
    {
        SPtr<ClassType> Class = Target->ExprType->ActuallyAs<ClassType>();
        FindMemberResult Found = Class->FindMember(Node->NameAfter, true);
        if(Found.IsClassMember && Found.FromClass != nullptr)
        {
            if(Found.FromClass->Type == Member_Variant)
            {
                if((Found.FromClass->Flags & CMF_Static))
                {
                    Text->Append(LPClassHelper::MakeMemberName(Found.FromClass->Owner.Lock(), Found.FromClass, T("")));
                }
                else
                {   
                    Text->Merge(FromTop(Index, 0)).Append(T(".")).Append(Node->NameAfter);
                }
            }
            else if(Found.FromClass->Type == Member_Function)
            {
                SPtr<TextParagraph> Prestat = OutText.NewParagraph();

                if((Found.FromClass->Flags & CMF_Abstract )
                    || (Found.FromClass->Flags & CMF_Virtual )
                    || (Found.FromClass->Flags & CMF_Override )
                    || (Found.FromClass->Owner->IsExternal))
                {   
                    // When using 'super' to call a virtual function. It is not really a virtual function call
                    // Because we are clear which function should be called
                    // So just directly call
                    if(Node->Func->Is<ASuper>())
                    {
                        Text->Append(LPClassHelper::MakeMemberName(Found.FromClass->Owner.Lock(), Found.FromClass, T("")));
                        IsVirtual = false;
                    }
                    else
                    {
                        Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(LPClassHelper::InCodeMemberName(Found.FromClass));
                        IsVirtual = true;
                    }
                } 
            
                else 
                {
                    if(Found.FromClass->Flags & CMF_Constructor)
                        Text->Append(LPClassHelper::MakeConstructorName(Found.FromClass->Owner.Lock(), Found.FromClass->Name, T("impl")));
                    else
                        Text->Append(LPClassHelper::MakeMemberName(Found.FromClass->Owner.Lock(), Found.FromClass, T("")));
                }   
            }
        }
        else if(Found.IsClassMember == false && Found.FromInterface != nullptr)
        {
            // Interface, always in vtab
            Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(LPClassHelper::InCodeMemberName(Found.FromInterface));
            IsVirtual = true;
        }
        else
        {
            Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(Node->NameAfter);
            IsRawStyle = true;
        }
    }
    else if(Target->ExprType->ActuallyIs<InterfaceType>())
    {
        SPtr<InterfaceType> Interface = Target->ExprType->ActuallyAs<InterfaceType>();
        InterfaceMember* Found = Interface->FindMember(Node->NameAfter, true);
        if(Found != nullptr)
        {
            Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(LPClassHelper::InCodeMemberName(Found));
            IsVirtual = true;
        }
        else
        {
            Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(Node->NameAfter);
            IsRawStyle = true;
        }
    }
    else
    {
        Text->Merge(FromTop(Index, 0)).Append(T(":")).Append(Node->NameAfter);
        IsRawStyle = true;
    }

    Text->Append(T("("));
    if(IsVirtual == false && IsRawStyle == false)
    {
        Text->Merge(FromTop(Index, 0));
        if (Node->Params.Count() > 0)
        {
            Text->Append(T(", "));
        }
    }
    if(Node->Params.Count() > 0)
    {
        int Count = StackCount(Index) - 1;
        for(int i = 0; i < Count; i++)
        {
            Text->Merge(FromTop(Index, i + 1));
            if(i != Count - 1)
                Text->Append(T(", "));
        }
        
    }
    Text->Append(T(")"));
    

    ContentStack.PopTo(Index);
    return VS_Continue;
}    

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ANormalCall> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue; 
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ANormalCall> Node)
{
    int Index = IndexStack.PickPop();
    //ASSERT_CHILD_NUM(Index, Node->Params.Count() + 1);

    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;

    // a normal call in a member function should be check to see if it is calling another member function
    bool ConvertToMemberFunc = false;
    bool ConvertToMetatableCall = false;
    bool ConvertToCtor = false;
    bool NeedSelf = false;
    OLString MemberFuncName;

    OLString SelfName = CurrSelfName;

    if(Node->Func->Is<AVarRef>())
    {
        SPtr<SymbolScope> Scope = CurrScope;
        SPtr<Reference> Ref = CurrScope->FindRefByNode(Node->Func.Get());
        while(Scope != nullptr)
        {
            if(Scope->Type == ST_Class)
            {
                SPtr<ClassType> ScopeClass = Scope->OwnerTypeDesc->ActuallyAs<ClassType>();
                if(Ref->Decl != nullptr)
                {
                    FindMemberResult Found = ScopeClass->FindMemberByDeclNode(Ref->Decl->DeclNode.Lock(), true);

                    if(Found.IsClassMember && Found.FromClass != nullptr)
                    {
                        if(Found.FromClass->Type == Member_Function)
                        {
                            if(Found.FromClass->Owner->IsExternal)
                            {
                                ConvertToMetatableCall = true;
                                MemberFuncName = LPClassHelper::InCodeMemberName(Found.FromClass); // Node->Func->As<AVarRef>()->VarName;
                            }
                            else if((Found.FromClass->Flags & CMF_Abstract)
                                || (Found.FromClass->Flags & CMF_Virtual)
                                || (Found.FromClass->Flags & CMF_Override) )
                            {
                                ConvertToMetatableCall = true;
                                MemberFuncName = LPClassHelper::InCodeMemberName(Found.FromClass); //Node->Func->As<AVarRef>()->VarName;
                            }
                            else if(Found.FromClass->Flags & CMF_Constructor)
                            {
                                ConvertToCtor = true;
                                MemberFuncName = LPClassHelper::MakeConstructorName(Found.FromClass->Owner.Lock(), Found.FromClass->Name, T("impl"));
                            }
                            else 
                            {
                                ConvertToMemberFunc = true;
                                MemberFuncName = LPClassHelper::MakeMemberName(Found.FromClass->Owner.Lock(), Found.FromClass, T(""));
                                if((Found.FromClass->Flags & CMF_Static) == 0)
                                {
                                    NeedSelf = true;
                                }
                            }
                            SelfName = LPClassHelper::MakeSelfName(Found.FromClass->Owner.Lock());
                        }
                        break;
                    }
                }
            }
            else
            {
                if(Scope == Ref->DeclScope.Lock())
                {
                    break;
                }
            }

            Scope = Scope->Parent.Lock();
        }
    }
    if(ConvertToMemberFunc == true)
    {
        Text->Append(MemberFuncName);
        if(NeedSelf)
        {
            Text->AppendF(T("(%s"), SelfName.CStr());
        }
        else
        {
            Text->Append(T("("));
        }
    }
    else if(ConvertToMetatableCall)
    {
        Text->AppendF(T("%s:%s("), SelfName.CStr(), MemberFuncName.CStr());
    }
    else if(ConvertToCtor == true)
    {
        Text->Append(MemberFuncName);
        Text->AppendF(T("(%s"), SelfName.CStr());
    }
    else
    {
        Text->Merge(FromTop(Index, 0))
            .Append(T("("));
    }
    

    int ParamCount = StackCount(Index) - 1;
    for (int i = 0; i < ParamCount; i++)
    {
        if((i != 0) || NeedSelf == true)
            Text->Append(T(", "));
        Text->Merge(FromTop(Index, i + 1));
    }
    Text->Append(T(")"));
    ContentStack.PopTo(Index);
    return VS_Continue;
}


EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ABracketMember> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ABracketMember> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 2);

    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;
    Text->Merge(FromTop(Index, 0))
        .Append(T("["))
        .Merge(FromTop(Index, 1))
        .Append(T("]"));

    ContentStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ADotMember> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ADotMember> Node)
{
    int Index = IndexStack.PickPop();
    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;

    if(Node->Target->ExprType->ActuallyIs<EnumType>())
    {
        SPtr<EnumType> Enum = Node->Target->ExprType->ActuallyAs<EnumType>();
        int Val = Enum->GetItemValue(Node->Field);
        Text->AppendF(T("%d"), Val);
    }
    else if(Node->Target->ExprType->ActuallyIs<ClassType>())
    {
        SPtr<ClassType> Class = Node->Target->ExprType->ActuallyAs<ClassType>();
        FindMemberResult Found = Class->FindMember(Node->Field, true);
        if(Found.FromClass == nullptr)
        {
            Text->Merge(FromTop(Index, 0)).Append(T(".")).Append(Node->Field);
        }
        else if(Found.IsClassMember)
        {
            if(Found.FromClass->Type == Member_Variant)
            {
                if(Found.FromClass->Flags & CMF_Static)
                {
                    Text->AppendF(T("%s.%s"), LPClassHelper::MakeStaticTableName(Class, T("")).CStr(), LPClassHelper::InCodeMemberName(Found.FromClass).CStr() );
                    //Text->Append(MakeMemberName(Class, Found.FromClass->Name, T("")));
                }
                else
                    Text->Merge(FromTop(Index, 0)).Append(T(".")).Append(Node->Field);
            }
            else if(Found.FromClass->Type == Member_Function)
            {
                // only static function or constructor is possible
                if(Found.FromClass->Flags & CMF_Constructor)
                {
                    Text->Append(LPClassHelper::MakeConstructorName(Class, Found.FromClass->Name, T("")));
                }
                else
                {
                    if(Found.FromClass->Owner->IsExternal)
                    {
                        // external classes, keep the original calling style
                        Text->Merge(FromTop(Index, 0)).Append(T(".")).Append(LPClassHelper::InCodeMemberName(Found.FromClass));
                    }
                    else
                    {
                        Text->Append(LPClassHelper::MakeMemberName(Found.FromClass->Owner.Lock(), Found.FromClass, T("")));
                    }
                }
            }
        }
        else
        {
            // Interface has no member that can be access by '.'
            ERROR(LogLuaPlain, T("Interface access error"));
            
        }
    }
    else 
    {
        Text->Merge(FromTop(Index, 0)).Append(T(".")).Append(Node->Field);
    }
    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ATypeCast> Node)
{
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ATypeCast> Node)
{
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ASubexpr> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ASubexpr> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->OperandList.Count());

    SPtr<TextParagraph> Text = ContentStack[Index].CurrText;
    if(Node->FirstUniOp != UO_None)
    {
        TCHAR const *  UniOpText = T("");
        switch (Node->FirstUniOp)
        {
        case UO_Minus:  UniOpText = T("-");   break;
        case UO_Length:  UniOpText = T("#");   break;
        case UO_Not:  UniOpText = T("not ");   break;
        case UO_BNot:  UniOpText = T("~");   break;
        }
        Text->Append(UniOpText);
    }

    for(int i = 0; i < Node->OperandList.Count(); i++)
    {
        if(i != 0)
        {
            TCHAR const * OpText = T("");
            switch(Node->OperandList[i].Op)
            {
            case BO_Add:        OpText = T(" + ");  break;
            case BO_Sub:        OpText = T(" - ");  break;
            case BO_Mul:        OpText = T(" * ");  break;
            case BO_Div:        OpText = T(" / ");  break;
            case BO_IDiv:       OpText = T(" // ");  break;
            case BO_Cat:        OpText = T(" .. ");  break;
            case BO_And:        OpText = T(" and ");  break;
            case BO_Or:         OpText = T(" or ");  break;
            case BO_ShiftL:     OpText = T(" << ");  break;
            case BO_ShiftR:     OpText = T(" >> ");  break;
            case BO_Greater:    OpText = T(" > ");  break;
            case BO_Less:       OpText = T(" < ");  break;
            case BO_GreaterEqual:    OpText = T(" >= ");  break;
            case BO_LessEqual:  OpText = T(" <= ");  break;
            case BO_Power:      OpText = T(" ^ ");  break;
            case BO_NotEqual:   OpText = T(" ~= ");  break;
            case BO_Equal:      OpText = T(" == ");  break;
            }

            Text->Append(OpText);
        }

        Text->Merge(FromTop(Index, i));
    }

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<ATypeIdentity> Node)
{
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ATypeIdentity> Node)
{
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;    
}
EVisitStatus LuaPlainInterp::EndVisit(SPtr<ATypeIdentity> Node)
{
    int Index = IndexStack.PickPop();
    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AClass> Node)
{
    
    SPtr<TypeDescBase> Type = CurrScope->FindTypeByNode(Node.Get(), false);
    OL_ASSERT(Type->ActuallyIs<ClassType>());
    SPtr<ClassType> Class = Type->ActuallyAs<ClassType>();


    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);


    CurrSelfName = LPClassHelper::MakeSelfName(Class);
    SelfNameStack.Add(CurrSelfName);

    SPtr<LPClassHelper> Helper = new LPClassHelper(Class, OutText);
    
    Helper->Prepare();
    ClassHelper.Add(Helper);

    return ScopeVisitorBase::BeginVisit(Node);
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AClass> Node)
{
    int Index = IndexStack.PickPop();
    int SubtextCount = StackCount(Index);

    SPtr<LPClassHelper> Helper = ClassHelper.PickPop();

    //ContentStack[Index].CurrText->NewLine().AppendF(T("------------ Begin class: %s ----------------------"), Node->ClassName.CStr()).NewLine();

    SPtr<ClassType> Class = CurrScope->OwnerTypeDesc->ActuallyAs<ClassType>();
    SPtr<ClassType> BaseClass = Class->GetSuperClass();

    bool SeperateStaticBlock =  false;
    SPtr<FuncSigniture> OuterFunc = GetOuterFunc();
    if(OuterFunc == nullptr)
        SeperateStaticBlock = true;

    if(BaseClass != nullptr)
    {
        ClassLib.SaveClass(Helper, Class->UniqueName, BaseClass->UniqueName, SeperateStaticBlock);
    }
    else
    {
        ClassLib.SaveClass(Helper, Class->UniqueName, T(""), SeperateStaticBlock);
    }

    Helper->DoBuild();

    OLString HolderName;
    HolderName.Printf(T("__class_content_%s"), Class->UniqueName.CStr());
    ContentStack[Index].CurrText->Hold(HolderName);
    if (SeperateStaticBlock == false)
    {
        HolderName.Printf(T("__class_static_content_%s"), Class->UniqueName.CStr());
        ContentStack[Index].CurrText->Hold(HolderName);
    }
    else
    {
        HolderName.Printf(T("__class_static_content_%s"), Class->UniqueName.CStr());
        SPtr<TextParagraph> StaticHolder = OutText.NewParagraph();
        StaticHolder->Hold(HolderName);
        PendingStaticTable.Add(StaticTableInfo{StaticHolder, Class});
    }


    ContentStack.PopTo(Index);

    SelfNameStack.Pop();
    CurrSelfName = SelfNameStack.Top();

    return ScopeVisitorBase::EndVisit(Node);
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AClassVar> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AClassVar> Node)
{
    int Index = IndexStack.PickPop();
    int SubtextCount = StackCount(Index);
    ASSERT_CHILD_NUM(Index, Node->Decls.Count() + Node->Inits.Count());

    SPtr<TypeDescBase> Type = CurrScope->Parent.Lock()->FindTypeByNode(Node->Parent.Lock().Get(), false);
    OL_ASSERT(Type->ActuallyIs<ClassType>());
    SPtr<ClassType> Class = Type->ActuallyAs<ClassType>();

    if(Class->IsExternal == false)
    {
        
        if(Node->Inits.Count() > 0)
        {
            for(int i = 0; i < Node->Inits.Count(); i++)
            {
                int NodeIndex = i;
                int InitExprIndex = i + Node->Decls.Count();
                ClassHelper.Top()->AddFieldInit(Node->Decls[i], Node->Inits[i], FromTop(Index, InitExprIndex));
            }
        }
    }

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AClassContructor> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus LuaPlainInterp::EndVisit(SPtr<AClassContructor> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1); // The only one is AFuncBody

    SPtr<TypeDescBase> Type = CurrScope->Parent.Lock()->FindTypeByNode(Node->Parent.Lock().Get(), false);
    OL_ASSERT(Type->ActuallyIs<ClassType>());

    SPtr<ClassType> Class = Type->ActuallyAs<ClassType>();

    ContentStack[Index].CurrText->Indent()
        .AppendF(T("function %s"), LPClassHelper::MakeConstructorName(Class, Node->Name, T("impl")).CStr())
        .Merge(FromTop(Index, 0));   // AFuncBody

    ClassHelper.Top()->AddConstructor(Node, ContentStack[Index].CurrText);

    ContentStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ANormalMethod> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}
EVisitStatus LuaPlainInterp::EndVisit(SPtr<ANormalMethod> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1); // The only one is AFuncBody

    SPtr<TypeDescBase> Type = CurrScope->Parent.Lock()->FindTypeByNode(Node->Parent.Lock().Get(), false);
    OL_ASSERT(Type->ActuallyIs<ClassType>());

    SPtr<ClassType> Class = Type->ActuallyAs<ClassType>();
    if(Node->Modifier->IsAbstract == false && Class->IsExternal == false)
    {
        FindMemberResult Found = Class->FindMember(Node->Name, false);
        OL_ASSERT(Found.IsClassMember && Found.FromClass != nullptr);
        
        // OLString InCodeName = Node->Name;
        // SPtr<DeclearAttributes> Attrib = Found.FromClass->MemberAttrib.Lock();
        // if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
        // {
        //     InCodeName = Attrib->GetString(T("lua_alias"));
        // }

        ContentStack[Index].CurrText->Indent()
            .AppendF(T("function %s"), LPClassHelper::MakeMemberName(Class, Found.FromClass, T("")).CStr())
            .Merge(FromTop(Index, 0));   // AFuncBody

        ClassHelper.Top()->AddNormalMethod(Node, ContentStack[Index].CurrText);
    }   
    ContentStack.PopTo(Index);

    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AVarDecl> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AVarDecl> Node)
{
    int Index = IndexStack.PickPop();
    ContentStack[Index].CurrText->Append(Node->VarName);

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AInterface> Node)
{
    return ScopeVisitorBase::BeginVisit(Node);
}
EVisitStatus LuaPlainInterp::EndVisit(SPtr<AInterface> Node)
{
    return ScopeVisitorBase::EndVisit(Node);
}



EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AFuncBody> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AFuncBody> Node)
{
    int Index = IndexStack.PickPop();
    if(Node->IsDeclearOnly)
    {
        ContentStack.PopTo(Index);
        return VS_Continue;
    }

    int SubtextCount = StackCount(Index);
    ASSERT_CHILD_NUM(Index, Node->Params.Count() + 1); // + 1 is the block

    // Check IsInstanceMemberFunc
    bool IsInstanceMemberFunc = false;
    if(Node->Parent->Is<AMethod>())
    {
        SPtr<TypeDescBase> Type = CurrScope->Parent.Lock()->FindTypeByNode(CurrScope->Owner.Lock().Get(), false);
        OL_ASSERT(Type->ActuallyIs<ClassType>());

        SPtr<ClassType> Class = Type->ActuallyAs<ClassType>();
        FindMemberResult Found = Class->FindMemberByDeclNode(Node->Parent.Lock(), false);
        OL_ASSERT(Found.IsClassMember == true);
        if(Found.FromClass == nullptr)
        {
            ERROR(LogLuaPlain, T("Cannot find member of node (%d,%d)"), Node->Line.Line, Node->Line.Col);
        }
        else
        {
            if((Found.FromClass->Flags & CMF_Static) == 0)
                IsInstanceMemberFunc = true;
        }
    }


    // Write content
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    if(IsInstanceMemberFunc)
        CurrText->AppendF(T("(%s"), CurrSelfName.CStr());
    else
        CurrText->Append(T("("));
    for(int i = 0; i < Node->Params.Count(); i++)
    {
        if(i != 0 || IsInstanceMemberFunc)
            CurrText->Append(T(", ")).Merge(FromTop(Index, i));
        else
            CurrText->Merge(FromTop(Index, i));

    }
    CurrText->Append(T(")")).NewLine().Merge(FromTop(Index, Node->Params.Count())).Indent().Append(T("end")).NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;

}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ALocal> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ALocal> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Decls.Count() + Node->Inits.Count());

    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;
    CurrText->Indent().Append(T("local "));
    for(int i = 0; i < Node->Decls.Count(); i++)
    {
        if(i == 0)
            CurrText->Merge(FromTop(Index, i));
        else
            CurrText->Append(T(", ")).Merge(FromTop(Index, i));
    }

    if(Node->Inits.Count() > 0)
    {
        CurrText->Append(T(" = "));
        for(int i = 0; i < Node->Inits.Count(); i++)
        {
            if(i == 0)
                CurrText->Merge(FromTop(Index, Node->Decls.Count() + i));
            else
                CurrText->Append(T(", ")).Merge(FromTop(Index, Node->Decls.Count() + i));
        }
    }
    CurrText->NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AGlobal> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AGlobal> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Decls.Count() + Node->Inits.Count());

    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;
    CurrText->Indent();
    for(int i = 0; i < Node->Decls.Count(); i++)
    {
        if(i == 0)
            CurrText->Merge(FromTop(Index, i));
        else
            CurrText->Append(T(", ")).Merge(FromTop(Index, i));
    }

    if(Node->Inits.Count() > 0)
    {
        CurrText->Append(T(" = "));
        for(int i = 0; i < Node->Inits.Count(); i++)
        {
            if(i == 0)
                CurrText->Merge(FromTop(Index, Node->Decls.Count() + i));
            else
                CurrText->Append(T(", ")).Merge(FromTop(Index, Node->Decls.Count() + i));
        }
    }
    CurrText->NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}



EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AFuncDef> Node)
{

    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AFuncDef> Node)
{
    int Index = IndexStack.PickPop();
    if(Node->IsExtern == false)
    {
        OLString InCodeName = Node->Name;
        SPtr<Declearation> Decl = CurrScope->FindDeclByNode(Node.Get());
        if(Decl != nullptr)
        {
            SPtr<DeclearAttributes> Attrib = Decl->Attrib;
            if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
            {
                InCodeName = Attrib->GetString(T("lua_alias"));
            }
        
        }

        SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

        if(Node->Owner == nullptr)
        {
            CurrText->Indent().AppendF(T("function %s"), InCodeName.CStr())
                .Merge(FromTop(Index, 0)).NewLine();
        }
        else
        {
            CurrText->Indent().Append(T("function ")).Merge(FromTop(Index, 0))
                .Append(Node->NeedSelf? T(":"):T(".")).Append(InCodeName.CStr())
                .Merge(FromTop(Index, 1));
        }
    }
    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AFuncExpr> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AFuncExpr> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("function ")).Merge(FromTop(Index, 0));
    
    ContentStack.PopTo(Index);
    return VS_Continue;
}


EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AAssignment> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AAssignment> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->LeftExprs.Count() + Node->RightExprs.Count());

    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;
    CurrText->Indent();
    for(int i = 0; i < Node->LeftExprs.Count(); i++)
    {
        if(i == 0)
            CurrText->Merge(FromTop(Index, i));
        else
            CurrText->Append(T(", ")).Merge(FromTop(Index, i));
    }

    CurrText->Append(T(" = "));
    for(int i = 0; i < Node->RightExprs.Count(); i++)
    {
        if(i == 0)
            CurrText->Merge(FromTop(Index, Node->LeftExprs.Count() + i));
        else
            CurrText->Append(T(", ")).Merge(FromTop(Index, Node->LeftExprs.Count() + i));
    }

    CurrText->NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<AVarRef> Node)
{
    SPtr<TextParagraph> CurrText = OutText.NewParagraph();
    SPtr<Reference> Ref = CurrScope->FindRefByNode(Node.Get());
    if(Ref == nullptr)
    {
        ERROR(LogLuaPlain, T("Unresolved ref at (%d, %d)"), Node->Line.Line, Node->Line.Col);
        CurrText->AppendF(T("$REF(%s)"), Node->VarName.CStr());
        ContentStack.Add(NodeGen(Node, CurrText));
        return VS_Continue;
    }

    SPtr<Declearation> Decl = Ref->Decl.Lock();
    SPtr<SymbolScope> Scope = CurrScope;
    while(Scope != nullptr)
    {
        if(Scope->Type == ST_Class)
        {
            SPtr<ClassType> ScopeClass = Scope->OwnerTypeDesc->ActuallyAs<ClassType>();
            if(Decl != nullptr)
            {
                FindMemberResult Found = ScopeClass->FindMemberByDeclNode(Decl->DeclNode.Lock(), true);
                if(Found.IsClassMember && Found.FromClass != nullptr)
                {
                    OLString InCodeName = LPClassHelper::InCodeMemberName(Found.FromClass);

                    OLString SelfName = LPClassHelper::MakeSelfName(ScopeClass);
                    if (Found.FromClass->Flags & CMF_Static)
                        CurrText->AppendF(T("%s.%s"), LPClassHelper::MakeStaticTableName(ScopeClass, T("")).CStr(), InCodeName.CStr());
                    else
                        CurrText->AppendF(T("%s.%s"), SelfName.CStr(), InCodeName.CStr());
                    break;
                }
            }
            
        }
        else
        {
            
            if(Scope == Ref->DeclScope.Lock())
            {
                OLString InCodeName = Node->VarName;
                SPtr<Declearation> Decl = Ref->Decl.Lock();
                // Using class name while accessing static member is handled here
                if(Decl != nullptr)
                {
                    SPtr<DeclearAttributes> Attrib = Decl->Attrib;
                    if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
                        InCodeName = Attrib->GetString(T("lua_alias"));
                }
                CurrText->Append(InCodeName.CStr());
                break;
            }
        }
        Scope = Scope->Parent.Lock();
    }

    if(Scope == nullptr)
    {
        OLString InCodeName = Node->VarName;
        SPtr<Declearation> Decl = Ref->Decl.Lock();
        if(Decl != nullptr)
        {
            SPtr<DeclearAttributes> Attrib = Decl->Attrib;
            if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
                InCodeName = Attrib->GetString(T("lua_alias"));
        }

        CurrText->Append(InCodeName.CStr());
    }


    ContentStack.Add(NodeGen(Node, CurrText));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ACallStat> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ACallStat> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 1);

    ContentStack[Index].CurrText->Indent().Merge(FromTop(Index, 0)).NewLine();
    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AForList> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AForList> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->VarList.Count() + 2);
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("for "));
    for(int i = 0; i < Node->VarList.Count(); i++)
    {
        if(i != 0)
            CurrText->Append(T(", "));
        CurrText->Merge(FromTop(Index, i));
    }

    SPtr<TextParagraph> IterText = OutText.NewParagraph();
    SPtr<TypeDescBase> IterType = Node->Iterator->ExprType.Lock();
    if(IterType->ActuallyIs<TupleType>())
    {
        IterText->Merge(FromTop(Index, Node->VarList.Count()));
    }
    else
    {
        IterText->Append(T("ipairs(")).Merge(FromTop(Index, Node->VarList.Count())).Append(T(")"));
    }

    CurrText->Append(T(" in ")).Merge(IterText).Append(T(" do")).NewLine()
        .Merge(FromTop(Index, Node->VarList.Count() + 1))
        .Indent().Append(T("end")).NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AForNum> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AForNum> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Step == nullptr ? 4 : 5);
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("for ")).Merge(FromTop(Index, 0)).Append(T(" = "))
        .Merge(FromTop(Index, 1)).Append(T(", ")).Merge(FromTop(Index, 2));

    int MainBlockIndex = 3;
    if(Node->Step != nullptr)
    {
        CurrText->Append(T(", ")).Merge(FromTop(Index, 3));
        MainBlockIndex = 4;
    }
    CurrText->Append(T(" do")).NewLine()
        .Merge(FromTop(Index, MainBlockIndex))
        .Indent().Append(T("end")).NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AIfStat> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AIfStat> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 2 + Node->ElseIfBlocks.Count() * 2 + (Node->ElseBlock == nullptr ? 0 : 1));
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("if ")).Merge(FromTop(Index, 0)).Append(T(" then")).NewLine()
        .Merge(FromTop(Index, 1));
    
    for(int i = 0; i < Node->ElseIfBlocks.Count(); i++)
    {
        int SubIndex = 2 + i * 2;
        CurrText->Indent().Append(T("elseif ")).Merge(FromTop(Index, SubIndex)).Append(T(" then")).NewLine()
            .Merge(FromTop(Index, SubIndex + 1));
    }

    if(Node->ElseBlock != nullptr)
    {
        int ElseIndex =  2 + Node->ElseIfBlocks.Count() * 2 ;
        CurrText->Indent().Append(T("else")).NewLine().Merge(FromTop(Index, ElseIndex));
    }

    CurrText->Indent().Append(T("end")).NewLine();
    
    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<ARepeat> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<ARepeat> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 2);
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("repeat")).NewLine().Merge(FromTop(Index, 0))
        .Indent().Append(T("until ")).Merge(FromTop(Index, 1));

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AWhile> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AWhile> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, 2);
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("while ")).Merge(FromTop(Index, 0)).Append(T(" do")).NewLine()
        .Merge(FromTop(Index, 1))
        .Indent().Append(T("end")).NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::BeginVisit(SPtr<AReturn> Node)
{
    ContentStack.Add(NodeGen(Node, OutText.NewParagraph()));
    IndexStack.Add(ContentStack.Count() - 1);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::EndVisit(SPtr<AReturn> Node)
{
    int Index = IndexStack.PickPop();
    ASSERT_CHILD_NUM(Index, Node->Ret.Count());
    SPtr<TextParagraph> CurrText = ContentStack[Index].CurrText;

    CurrText->Indent().Append(T("return "));

    for(int i = 0; i < Node->Ret.Count(); i++)
    {
        if(i != 0)
            CurrText->Append(T(", "));
        CurrText->Merge(FromTop(Index, i));
    }
    CurrText->NewLine();

    ContentStack.PopTo(Index);
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<ABreak> Node)
{
    SPtr<TextParagraph> CurrText = OutText.NewParagraph();
    CurrText->Indent().Append(T("break")).NewLine();
    ContentStack.Add(NodeGen(Node, CurrText));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<AGoto> Node)
{
    SPtr<TextParagraph> CurrText = OutText.NewParagraph();
    CurrText->Indent().AppendF(T("goto %s"), Node->Label.CStr()).NewLine();
    ContentStack.Add(NodeGen(Node, CurrText));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<ALabel> Node)
{
    SPtr<TextParagraph> CurrText = OutText.NewParagraph();
    CurrText->Indent().AppendF(T("::%s"), Node->Name.CStr()).NewLine();
    ContentStack.Add(NodeGen(Node, CurrText));
    return VS_Continue;
}

EVisitStatus LuaPlainInterp::Visit(SPtr<AVariableParamRef> Node)
{
    SPtr<TextParagraph> CurrText = OutText.NewParagraph();
    CurrText->Append(T("..."));
    ContentStack.Add(NodeGen(Node, CurrText));
    return VS_Continue;
}

}