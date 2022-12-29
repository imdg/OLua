/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "LPBuiltInFuncInterp.h"
#include "BuiltinLib.h"
#include "FuncGenBase.h"
#include "VisitorBase.h"
#include "LuaPlainInterp.h"
#include "AstInclude.h"
#include "EnumType.h"

namespace OL
{
LPBuiltinFuncGen::LPBuiltinFuncGen(OLFunc<LPBuiltInFuncInterp::GenCallBack> InOnCall)
{
    OnCall = InOnCall;
}

bool LPBuiltinFuncGen::DoCodeGen(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    if(OnCall != nullptr)
    {
        return OnCall(Interp, Result, BaseIndex, Node);
    }
    return false;
}


RTTI_BEGIN_INHERITED(LPBuiltinFuncGen, FuncGenBase)

RTTI_END(LPBuiltinFuncGen)


bool CodeGen_IntrinsiclToString(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("tostring(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;

    return true;
}


bool CodeGen_Float_Floor(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("math.floor(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}
bool CodeGen_Float_Ceil(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("math.ceil(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_ToInteger(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("math.tointeger(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_ToFloat(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("(tonumber(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(") + 0.0)"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_ToBool(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("olua_ext.string_to_bool(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_Len(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("string.len(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_Upper(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("string.upper(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}
bool CodeGen_String_Lower(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("string.lower(")).Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_Sub(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("string.sub(")).Merge(Interp.FromTop(BaseIndex, 0));
    int Count = Interp.StackCount(BaseIndex) - 1;
    for(int i = 0; i < Count; i++)
    {
        Text->Merge(Interp.FromTop(BaseIndex, i + 1));
        if(i != Count - 1)
            Text->Append(T(", "));
    }
    Text->Append(T(")"));

    Result.CurrText = Text;
    return true;
}

bool CodeGen_String_GetChar(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("string.sub("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(", "))
        .Merge(Interp.FromTop(BaseIndex, 1)).Append(T(", 1)"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Array_Add(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("table.insert("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(", "))
        .Merge(Interp.FromTop(BaseIndex, 1)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Array_Insert(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("table.insert("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(", "))
        .Merge(Interp.FromTop(BaseIndex, 1)).Append(T(", "))
        .Merge(Interp.FromTop(BaseIndex, 2)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Array_RemoveAt(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("table.remove("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(", "))
        .Merge(Interp.FromTop(BaseIndex, 1)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Array_Clear(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("olua_ext.table_clear("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}


bool CodeGen_Map_Add(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Merge(Interp.FromTop(BaseIndex, 0))
        .Append(T("[")).Merge(Interp.FromTop(BaseIndex, 1)).Append(T("] ="))
        .Merge(Interp.FromTop(BaseIndex, 2));
    
    Result.CurrText = Text;
    return true;
}


bool CodeGen_Map_Remove(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Merge(Interp.FromTop(BaseIndex, 0))
        .Append(T("[")).Merge(Interp.FromTop(BaseIndex, 1)).Append(T("] = nil"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Map_Clear(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    Text->Append(T("olua_ext.table_clear("))
        .Merge(Interp.FromTop(BaseIndex, 0)).Append(T(")"));
    Result.CurrText = Text;
    return true;
}

SPtr<EnumType> GetEnumTypeFromInterp(LuaPlainInterp& Interp, int BaseIndex, int StackId)
{
    SPtr<ABase> OwnerNode = Interp.NodeFromTop(BaseIndex, StackId).Node;
    if(OwnerNode->Is<AExpr>() == false)
        return nullptr;
    SPtr<AExpr> ExprNode = OwnerNode.PtrAs<AExpr>();
    if(ExprNode->ExprType == nullptr)
        return nullptr;

    // For colon calls
    if(ExprNode->ExprType->Is<EnumType>() )
    {
        SPtr<EnumType> Enum = ExprNode->ExprType.Lock().PtrAs<EnumType>();
        return Enum;
    }

    // For dot member. The ExprType is the dereferenced function.
    // So query if the expr is a dot member and is dereferenced from an enum
    if(ExprNode->Is<ADotMember>())
    {
        SPtr<ADotMember> Dot = ExprNode.PtrAs<ADotMember>();
        if(Dot->Target->ExprType != nullptr && Dot->Target->ExprType->Is<EnumType>())
        {
            SPtr<EnumType> Enum = Dot->Target->ExprType.Lock().PtrAs<EnumType>();
            return Enum;
        }
    }

    return nullptr;
}

bool CodeGen_Enum_Items(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    SPtr<EnumType> Enum = GetEnumTypeFromInterp(Interp, BaseIndex, 0);
    if(Enum == nullptr)
        return false;

    Text->AppendF(T("pairs(%s)"), Interp.MakeEnumStaticTableName(Enum).CStr());
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Enum_ToString(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    OL_ASSERT(Node->Is<AColonCall>());
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();

    SPtr<EnumType> Enum = GetEnumTypeFromInterp(Interp, BaseIndex, 0);
    if(Enum == nullptr)
        return false;

    Text->AppendF(T("olua_ext.enum_tostring(%s, "), Interp.MakeEnumStaticTableName(Enum).CStr())
        .Merge(Interp.FromTop(BaseIndex, 0))
        .Append(T(")"));
    Result.CurrText = Text;
    return true;
}

bool CodeGen_Enum_Parse(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node)
{
    SPtr<TextParagraph> Text = Interp.OutText.NewParagraph();
    SPtr<EnumType> Enum = GetEnumTypeFromInterp(Interp, BaseIndex, 0);
    if(Enum == nullptr)
        return false;

    Text->AppendF(T("olua_ext.enum_parse(%s, "), Interp.MakeEnumStaticTableName(Enum).CStr())
        .Merge(Interp.FromTop(BaseIndex, 1))
        .Append(T(")"));
    Result.CurrText = Text;
    return true;
}


void LPBuiltInFuncInterp::Init()
{
    BuiltinLib& Lib = BuiltinLib::GetInst();
    // Int
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_int, T("tostring")),  CodeGen_IntrinsiclToString);

    // Float
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_float, T("tostring")),    CodeGen_IntrinsiclToString);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_float, T("floor")),       CodeGen_Float_Floor);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_float, T("ceil")),        CodeGen_Float_Ceil);

    // Bool
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_bool, T("tostring")),        CodeGen_IntrinsiclToString);

    // String
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("tointeger")),    CodeGen_String_ToInteger);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("tofloat")),      CodeGen_String_ToFloat);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("tobool")),       CodeGen_String_ToBool);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("len")),          CodeGen_String_Len);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("lower")),        CodeGen_String_Upper);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("upper")),        CodeGen_String_Lower);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("sub")),          CodeGen_String_Sub);
    RegisterCodeGen(Lib.GetIntrinsicMethod(IT_string, T("getchar")),      CodeGen_String_GetChar);

    //Array
    RegisterCodeGen(Lib.GetArrayMethod(T("add")),      CodeGen_Array_Add);
    RegisterCodeGen(Lib.GetArrayMethod(T("insert")),      CodeGen_Array_Insert);
    RegisterCodeGen(Lib.GetArrayMethod(T("remove_at")),   CodeGen_Array_RemoveAt);
    RegisterCodeGen(Lib.GetArrayMethod(T("clear")),    CodeGen_Array_Clear);

    //Map
    RegisterCodeGen(Lib.GetMapMethod(T("add")),      CodeGen_Map_Add);
    RegisterCodeGen(Lib.GetMapMethod(T("remove")),   CodeGen_Map_Remove);
    RegisterCodeGen(Lib.GetMapMethod(T("clear")),    CodeGen_Map_Clear);

    //Enum
    RegisterCodeGen(Lib.GetEnumBaseMethod(T("items")),      CodeGen_Enum_Items);
    RegisterCodeGen(Lib.GetEnumBaseMethod(T("tostring")),   CodeGen_Enum_ToString);
    RegisterCodeGen(Lib.GetEnumBaseMethod(T("parse")),      CodeGen_Enum_Parse);
}   



void LPBuiltInFuncInterp::RegisterCodeGen(SPtr<FuncSigniture> Func, OLFunc<GenCallBack> Generator)
{
    SPtr<LPBuiltinFuncGen> NewCodeGen = new LPBuiltinFuncGen(Generator);
    Func->OverrideGenerater = NewCodeGen;
}

LPBuiltInFuncInterp* LPBuiltInFuncInterp::Inst = nullptr;
LPBuiltInFuncInterp& LPBuiltInFuncInterp::GetInst()
{
    if(Inst == nullptr)
        Inst = new LPBuiltInFuncInterp();
    return *Inst;
}


}