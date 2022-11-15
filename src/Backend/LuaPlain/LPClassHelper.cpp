#include "LPClassHelper.h"
#include "ClassType.h"
#include "AClass.h"
#include "AClassVar.h"
#include "AVarDecl.h"
#include "AExpr.h"
#include "AClassContructor.h"
#include "AFuncBody.h"
#include "ANormalMethod.h"
#include "SymbolScope.h"
#include "InterfaceType.h"
namespace OL
{

#define SELF_OBJ_NAME T("__self_obj_")


OLString LPClassHelper::InCodeMemberName(ClassMemberDesc* Member)
{
    OLString InCodeMemberName = Member->Name;
    SPtr<DeclearAttributes> Attrib = Member->MemberAttrib.Lock();
    if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
        InCodeMemberName = Attrib->GetString(T("lua_alias"));
    return InCodeMemberName;
}

OLString LPClassHelper::InCodeMemberName(InterfaceMember* Member)
{
    OLString InCodeMemberName = Member->Name;
    SPtr<DeclearAttributes> Attrib = Member->MemberAttrib.Lock();
    if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
        InCodeMemberName = Attrib->GetString(T("lua_alias"));
    return InCodeMemberName;
}

OLString LPClassHelper::InCodeClassName(SPtr<ClassType> Class)
{
    OLString InCodeClassName = Class->UniqueName;
    SPtr<DeclearAttributes> Attrib = Class->ClassAttrib.Lock();
    if(Attrib != nullptr && Attrib->HasItem(T("lua_alias"), IT_string))
        InCodeClassName = Attrib->GetString(T("lua_alias"));
    return InCodeClassName;
}

OLString CombineMemberName(OLString ClassName, OLString MemberName, OLString Suffix)
{
    OLString Ret;
    Ret.Printf(T("__CM_%s_%s_%s"), ClassName.CStr(), MemberName.CStr(), Suffix.CStr());
    return Ret;
}

OLString LPClassHelper::MakeMemberName(SPtr<ClassType> Class, ClassMemberDesc* Member, OLString Suffix)
{
    
    return CombineMemberName(InCodeClassName(Class), InCodeMemberName(Member), Suffix);
}

OLString LPClassHelper::MakeConstructorName(SPtr<ClassType> Class, OLString CtorName, OLString Suffix)
{
    OLString Ret;
    Ret.Printf(T("__CM_%s_ctor_%s_%s"), InCodeClassName(Class).CStr(), CtorName.CStr(), Suffix.CStr());
    return Ret;
}

OLString LPClassHelper::MakeStaticTableName(SPtr<ClassType> Class, OLString Suffix)
{
    OLString Ret;
    Ret.Printf(T("__CS_%s_%s"), InCodeClassName(Class).CStr(),  Suffix.CStr());
    return Ret;
}

OLString LPClassHelper::MakeSelfName(SPtr<ClassType> Class)
{
    OLString Ret;
    Ret.Printf(T("%s%d"), SELF_OBJ_NAME, Class->NestLevel );
    return Ret;
}


LPClassHelper::LPClassHelper(SPtr<ClassType> InClass, TextBuilder& InTextOwner)
    : Class(InClass), TextOwner(InTextOwner)
{
    CurrSelfName.Printf(T("%s%d"), SELF_OBJ_NAME, Class->NestLevel);
}


void LPClassHelper::BuildDefinitionItems()
{


    Class->ForAllMembers(false, [this] (ClassMemberDesc& Member) -> bool
    {
        if(Member.Type != Member_Variant)
            return true;

        SPtr<TextParagraph> VarText = TextOwner.NewParagraph();
        VarText->AppendF(T("[\"%s\"] = "), Member.Name.CStr()).Hold(T("__init_expr"));
        if((Member.Flags & CMF_Static) == 0)
            InstancedDefines.Add(DefinitionItem{VarText, Member.Name});
        else
            StaticDefines.Add(DefinitionItem{VarText, Member.Name});
        return true;
    });

}

void LPClassHelper::LoadBaseClass(SPtr<LPClassHelper> BaseClassHelper)
{
    for(int i = 0; i < BaseClassHelper->InstancedDefines.Count(); i++)
    {
        DefinitionItem& BaseItem = BaseClassHelper->InstancedDefines[i];
        InstancedDefines.Add(DefinitionItem{BaseItem.Text->Duplicate(), BaseItem.MemberName});

    }

    SavedHiddenCtorName = BaseClassHelper->SavedHiddenCtorName;
}

// This is called after all text is built
// And is ensured that base class called before child class
void LPClassHelper::ReplaceBaseDefinition(SPtr<LPClassHelper> BaseClassHelper)
{
    int OriginDefCount = InstancedDefines.Count();
    // Add base class def item to self
    // Not for self, but for child classes
    for(int i = BaseClassHelper->InstancedDefines.Count() - 1; i >= 0 ; i--)
    {
        DefinitionItem& BaseItem = BaseClassHelper->InstancedDefines[i];
        InstancedDefines.Insert(DefinitionItem{BaseItem.Text->Duplicate(), BaseItem.MemberName}, 0);

    }

    SPtr<TextParagraph> BaseDefText = BuildInstDefInsideBlock(BaseClassHelper->InstancedDefines);
    if(OriginDefCount > 0)
        BaseDefText->Append(T(",")).NewLine();
        
    OLString HolderName;
    HolderName.Printf(T("__base_class_def_%s"), Class->UniqueName.CStr());
    MainBlock->ReplaceHolder(HolderName, BaseDefText, true);

    HolderName.Printf(T("__base_hidden_ctor_call_%s"), Class->UniqueName.CStr());
    SPtr<TextParagraph> BaseHiddenCtor = TextOwner.NewParagraph();
    if(BaseClassHelper->SavedHiddenCtorName != T(""))
    {
        BaseHiddenCtor->Indent().AppendF(T("%s(%s)"), BaseClassHelper->SavedHiddenCtorName .CStr(), CurrSelfName.CStr()).NewLine();
    }
    MainBlock->ReplaceHolder(HolderName, BaseHiddenCtor, true);

    if(SavedHiddenCtorName == T(""))
        SavedHiddenCtorName = BaseClassHelper->SavedHiddenCtorName;
}

void LPClassHelper::BuildVTableItems()
{

    OLList<FindMemberResult> VirtualFuncs;
    Class->FindVirtualFunc(VirtualFuncs);

    for(int i = 0; i < VirtualFuncs.Count(); i++)
    {
        SPtr<TextParagraph> ItemText = TextOwner.NewParagraph();
        ItemText->AppendF(T("[\"%s\"] = %s")
            , VirtualFuncs[i].FromClass->Name.CStr()
            , MakeMemberName(VirtualFuncs[i].FromClass->Owner.Lock(), VirtualFuncs[i].FromClass, T("")).CStr() );
        
        VTableItems.Add(ItemText);
    }
}

OLString LPClassHelper::CheckOperator(OLString MetaName)
{
    FindMemberResult Member = Class->FindMember(MetaName, true);
    if(Member.IsClassMember && (Member.FromClass->Flags & CMF_Abstract) == false)
    {
        return MakeMemberName(Member.FromClass->Owner.Lock(), Member.FromClass, T(""));
    }
    return T("");
}

void LPClassHelper::BuildOperatorOverride()
{
    static const TCHAR* Operators[] =  {
        T("__add"), T("__sub"), T("__mul"), T("__div"), T("__mod"), T("__unm"), T("__concat"), T("__eq"), T("__lt"), T("__le"), T("__tostring")
    };
    int Count = sizeof(Operators) / sizeof(TCHAR*);

    for(int i = 0; i < Count; i++)
    {
        OLString OpFunc = CheckOperator(Operators[i]);
        if(OpFunc != T(""))
        {
            OperatorOverride& NewOp = OpInfo.AddConstructed();
            NewOp.OpMetaName = Operators[i];
            NewOp.OpFuncName = OpFunc;
        }
    }

}

void LPClassHelper::AddFieldInit(SPtr<AVarDecl> DeclNode, SPtr<AExpr> InitNode, SPtr<TextParagraph> ExprText)
{
    FindMemberResult Member = Class->FindMember(DeclNode->VarName, true);
    if(InitNode->IsConstant())
    {
        DefinitionItem* Definition = FindDefinition(DeclNode->VarName);
        if(Definition != nullptr)
        {
            Definition->Text->ReplaceHolder(T("__init_expr"), ExprText, false);
            return;
        }
        
    }

    
    SPtr<TextParagraph> ItemText = TextOwner.NewParagraph();
    

    if (Member.FromClass->Flags & CMF_Static)
    {
        OLString StaticName = MakeStaticTableName(Class, T(""));
        ItemText->AppendF(T("%s.%s = "), StaticName.CStr(), DeclNode->VarName.CStr()).Merge(ExprText);
        StaticInit.Add(ItemText);
    }
    else
    {
        ItemText->AppendF(T("%s.%s = "), CurrSelfName.CStr(), DeclNode->VarName.CStr()).Merge(ExprText);
        InstancedInit.Add(ItemText);
    }
}


LPClassHelper::DefinitionItem* LPClassHelper::FindDefinition(OLString Name)
{
    for(int i = 0; i < InstancedDefines.Count(); i++)
    {
        if(InstancedDefines[i].MemberName == Name)
            return &(InstancedDefines[i]);
    }

    for(int i = 0; i < StaticDefines.Count(); i++)
    {
        if(StaticDefines[i].MemberName == Name)
            return &(StaticDefines[i]);
    }

    return nullptr;
}

void LPClassHelper::AddConstructor(SPtr<AClassContructor> Node, SPtr<TextParagraph> ImplText)
{
    CtorInfo Info;
    Info.Impl = ImplText;
    

    SPtr<TextParagraph> Wrapper = TextOwner.NewParagraph();
    SPtr<TextParagraph> ParamList = TextOwner.NewParagraph();
    SPtr<TextParagraph> Call = TextOwner.NewParagraph();
    bool HasParam = false;
    for(int i = 0; i < Node->Body->Params.Count(); i++)
    {
        if(i != 0)
            ParamList->Append(T(", "));
        ParamList->Append(Node->Body->Params[i]->VarName);
        HasParam = true;
    }

    OLString WrapperName = MakeConstructorName(Class, Node->Name, T(""));
    OLString ImplName = MakeConstructorName(Class, Node->Name, T("impl"));

    Wrapper->Indent().AppendF(T("function %s("), WrapperName.CStr()).Merge(ParamList->Duplicate()).Append(T(")")).NewLine()
        .IndentInc().Hold(T("__instanced_def"))
        .Hold(T("__hidden_init_call"));
    
    //Call->Indent().AppendF(T("%s(%s"), ImplName.CStr(), CurrSelfName.CStr());
    Wrapper->Indent().AppendF(T("%s(%s"), ImplName.CStr(), CurrSelfName.CStr());
    if(HasParam)
        Wrapper->Append(T(", ")).Merge(ParamList);

    Wrapper->Append(T(")")).NewLine()
        .Indent().AppendF(T("return %s"), CurrSelfName.CStr()).NewLine();
    
    Wrapper->IndentDec().Indent().Append(T("end")).NewLine();
    
    Info.Wrapper = Wrapper;

    //Call->Append(T(")")).NewLine();

    
    

    Ctors.Add(Info);

}

void LPClassHelper::AddDefaultConstructor()
{
    CtorInfo Info;
    Info.Impl = TextOwner.NewParagraph(); //Empty text

    SPtr<TextParagraph> Wrapper = TextOwner.NewParagraph();
    Wrapper->Indent().AppendF(T("function %s("), MakeConstructorName(Class, T("new"), T("")).CStr()).Append(T(")")).NewLine()
        .IndentInc().Hold(T("__instanced_def"))
        .Hold(T("__hidden_init_call"));

    Wrapper->Indent().AppendF(T("return %s"), CurrSelfName.CStr()).NewLine();
    Wrapper->IndentDec()
        .Indent().Append(T("end")).NewLine();
    
    Info.Wrapper = Wrapper;
   

    Ctors.Add(Info);
}


void LPClassHelper::AddNormalMethod(SPtr<ANormalMethod> Node, SPtr<TextParagraph> Text)
{
    Methods.Add(Text);
}

SPtr<TextParagraph> LPClassHelper::BuildInstDefInsideBlock(OLList<DefinitionItem>& Defs)
{
    SPtr<TextParagraph> Ret = TextOwner.NewParagraph();

    for(int i = 0; i < Defs.Count(); i++)
    {
        if(i != 0 )
            Ret->Append(T(",")).NewLine();

        FindMemberResult Found = Class->FindMember(Defs[i].MemberName, true);
        SPtr<TextParagraph> DefVal = TextOwner.NewParagraph();
        DefVal->Append(Found.FromClass->DeclTypeDesc->DefaultValueText());

        Defs[i].Text->ReplaceHolder(T("__init_expr"), DefVal, false);
        Ret->Indent().Merge(Defs[i].Text->Duplicate());
    }

    return Ret;
}

SPtr<TextParagraph> LPClassHelper::BuildInstDefBlock()
{
    SPtr<TextParagraph> InstDef = TextOwner.NewParagraph();
    InstDef->Indent().AppendF(T("local %s = {"), CurrSelfName.CStr()).NewLine()
        .IndentInc();

    InstancedDefineCombined = BuildInstDefInsideBlock(InstancedDefines);

    OLString HolderName;
    HolderName.Printf(T("__base_class_def_%s"), Class->UniqueName.CStr());
    InstDef->Hold(HolderName). Merge(InstancedDefineCombined).NewLine().IndentDec(). Indent().Append(T("}")).NewLine();

    return InstDef;
}

SPtr<TextParagraph> LPClassHelper::BuildStaticDefBlock()
{
    OLString StaticName = MakeStaticTableName(Class, T(""));

    SPtr<TextParagraph> StaticDefText = TextOwner.NewParagraph();
    StaticDefText->Indent().AppendF(T("%s = {"), StaticName.CStr()).NewLine()
        .IndentInc();

    bool First = true;
    for(int i = 0; i < StaticDefines.Count(); i++)
    {
        if(First == false)
            StaticDefText->Append(T(", ")).NewLine();
        StaticDefText->Indent().Merge(StaticDefines[i].Text);
        First = false;
    }

    if(VTableItems.Count() > 0)
    {
        SPtr<TextParagraph> VTableText = TextOwner.NewParagraph();
        VTableText->Indent().Append(T("[\"vtable\"] = {")).NewLine()
            .IndentInc();

        for(int i = 0; i < VTableItems.Count(); i++)
        {
            if(i != 0)
                VTableText->Append(T(",")).NewLine();
            VTableText->Indent(). Merge(VTableItems[i]);
        }
        VTableText->NewLine().IndentDec().Indent().Append(T("}")).NewLine();

        if(First == false)
            StaticDefText->Append(T(", ")).NewLine();
        StaticDefText->Merge(VTableText);
    }
    else
    {
        StaticDefText->NewLine();
    }

    StaticDefText->IndentDec().Indent().Append(T("}")).NewLine();
    return StaticDefText;
}

SPtr<TextParagraph> LPClassHelper::BuildHiddenCtor()
{
    SPtr<TextParagraph> HiddenCtorContent = TextOwner.NewParagraph();
    CurrHiddenCtorName = MakeConstructorName(Class, T("ctor"), T("hidden"));

    // currently SavedHiddenCtorName is still the base class' value
    // if(SavedHiddenCtorName != T(""))
    // {
    //     HiddenCtorContent->Indent().AppendF(T("%s(%s)"), SavedHiddenCtorName.CStr(), CurrSelfName.CStr()).NewLine();
    // }
    OLString HolderName;
    HolderName.Printf(T("__base_hidden_ctor_call_%s"), Class->UniqueName.CStr());
    HiddenCtorContent->Hold(HolderName);


    if(InstancedInit.Count() > 0)
    {
        for(int i = 0; i < InstancedInit.Count(); i++)
        {
            HiddenCtorContent->Indent().Merge(InstancedInit[i]).NewLine();
        }
        // this hidden need to do real things, update SavedHiddenCtorName so that child classes can call
        SavedHiddenCtorName = CurrHiddenCtorName;
    }

    if(HiddenCtorContent->Head != nullptr)
    {
        SPtr<TextParagraph> HiddenCtor = TextOwner.NewParagraph();
        HiddenCtor->Indent().AppendF(T("function %s(%s)"), CurrHiddenCtorName.CStr(), CurrSelfName.CStr()).NewLine()
            .IndentInc()
            .Merge(HiddenCtorContent)
            .IndentDec().NewLine()
            .Indent().Append(T("end")).NewLine();
        return HiddenCtor;
    }

    CurrHiddenCtorName = T(""); // clear this
    return HiddenCtorContent; // null text
    
}

SPtr<TextParagraph> LPClassHelper::BuildStaticCtor()
{
    if(VTableItems.Count() == 0 && StaticDefines.Count() == 0)
        return TextOwner.NewParagraph();

    SPtr<TextParagraph> StaticDefText = BuildStaticDefBlock();

    if(StaticInit.Count() == 0)
        return StaticDefText;


    OLString StaticCtorName = CombineMemberName(Class->UniqueName, T("static_new"), T("hidden"));
    SPtr<TextParagraph> StaticCtorText = TextOwner.NewParagraph();
    StaticCtorText->Indent().AppendF(T("function %s()"), StaticCtorName.CStr()).NewLine()
        .IndentInc();

    for(int i = 0; i< StaticInit.Count(); i++)
    {
        StaticCtorText->Indent().Merge(StaticInit[i]).NewLine();
    }
    StaticCtorText->IndentDec().Indent().Append(T("end")).NewLine();
    

    StaticDefText->Merge(StaticCtorText);

    return StaticDefText;

}

SPtr<TextParagraph> LPClassHelper::BuildCtorBlock()
{
    SPtr<TextParagraph> HiddenCtor = BuildHiddenCtor();

    SPtr<TextParagraph> InstDef = BuildInstDefBlock();
    SPtr<TextParagraph> HiddenCtorCall = TextOwner.NewParagraph();
    if(VTableItems.Count() > 0 || OpInfo.Count() > 0)
    {
        
        // HiddenCtorCall->Indent()
        //     .AppendF(T("setmetatable(%s, { __index = %s.__vtable })")
        //         , CurrSelfName.CStr(), StaticName.CStr())
        //     .NewLine();
        
        HiddenCtorCall->Indent().AppendF(T("setmetatable(%s, { "), CurrSelfName.CStr()).NewLine().IndentInc();
        int ItemCount = 0;
        if(VTableItems.Count() > 0 )
        {
            ItemCount++;
            OLString StaticName = MakeStaticTableName(Class, T(""));
            HiddenCtorCall->Indent().AppendF(T("__index = %s.vtable"), StaticName.CStr());
        }
        if(OpInfo.Count() > 0)
        {
            for(int i = 0; i < OpInfo.Count(); i++)
            {
                if(ItemCount != 0)
                    HiddenCtorCall->Append(T(",")).NewLine();
                HiddenCtorCall->Indent().AppendF(T("%s = %s"), OpInfo[i].OpMetaName.CStr(), OpInfo[i].OpFuncName.CStr());
                ItemCount++;
            }
        }
        HiddenCtorCall->NewLine().IndentDec().Indent().Append(T("})")).NewLine();
    }
    if(CurrHiddenCtorName != T(""))
    {
        HiddenCtorCall->Indent()
            .AppendF(T("%s(%s)"), CurrHiddenCtorName.CStr(), CurrSelfName.CStr())
            .NewLine();
    }

    SPtr<TextParagraph> AllCtor = TextOwner.NewParagraph();
    AllCtor->Merge(HiddenCtor).NewLine();

    for(int i = 0; i < Ctors.Count(); i++)
    {
        AllCtor->Merge(Ctors[i].Impl).NewLine();

        Ctors[i].Wrapper->ReplaceHolder(T("__instanced_def"), InstDef->Duplicate(), false);
        AllCtor->Merge(Ctors[i].Wrapper).NewLine();
    }
    AllCtor->ReplaceHolder(T("__hidden_init_call"), HiddenCtorCall, true);

    return AllCtor;
}

void LPClassHelper::Prepare()
{
    if(Ctors.Count() == 0)
        AddDefaultConstructor();

    BuildDefinitionItems();
    BuildVTableItems();
    BuildOperatorOverride();
}

void LPClassHelper::DoBuild()
{
    if(Class->IsExternal)
    {
        MainBlock = TextOwner.NewParagraph();
        StaticBlock = TextOwner.NewParagraph();
        return;
    }

    SPtr<TextParagraph> CtorBlock = BuildCtorBlock();
    MainBlock = TextOwner.NewParagraph();

    MainBlock->Merge(CtorBlock);

    for(int i = 0; i < Methods.Count(); i++)
    {
        MainBlock->Merge(Methods[i]).NewLine();
    }

    StaticBlock = BuildStaticCtor();
    
}

}