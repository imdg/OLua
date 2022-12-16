/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "LPReflHelper.h"
#include "TextBuilder.h"
#include "ClassType.h"
#include "FuncSigniture.h"
#include "ArrayType.h"
#include "MapType.h"
#include "LPClassHelper.h"

namespace OL
{
#define BOOL_STR(boolvalue) ((boolvalue) ? T("true"): T("false"))

int ClassMemberFlagToAccess(uint Flag)
{
    if(Flag & CMF_Public)
        return 1;
    else if(Flag & CMF_Protected)
        return 2;
    else if(Flag & CMF_Protected)
        return 3;
    return 3;
}

SPtr<TextParagraph> LPReflHelper::BuildClassRefl(SPtr<ClassType> Class, TextBuilder& TextOwner)
{
    SPtr<TextParagraph> ReflText = TextOwner.NewParagraph();
    ReflText->Append(T("setmetatable({ ")).NewLine().IndentInc();

    ReflText->Indent().Append(T("[\"__members\"] = {")).IndentInc();
    bool IsFirst = true;
    Class->ForAllMembers(true, [&ReflText, &IsFirst, &TextOwner](ClassMemberDesc& Member) -> bool
    {
        if(IsFirst)
            ReflText->NewLine();
        else
            ReflText->Append(T(",")).NewLine();
        IsFirst = false;

        ReflText->Indent().Append(T("{")).NewLine().IndentInc();
        ReflText->Indent().AppendF(T("[\"__name\"] = \"%s\", "),          Member.Name.CStr()).NewLine();
        ReflText->Indent().AppendF(T("[\"__access\"] = %d, "),        ClassMemberFlagToAccess(Member.Flags)).NewLine();
        ReflText->Indent().AppendF(T("[\"__is_static\"] = %s, "),     BOOL_STR(Member.Flags & CMF_Static) ).NewLine();
        ReflText->Indent().AppendF(T("[\"__is_variant\"] = %s, "),    BOOL_STR(Member.Type == Member_Variant) ).NewLine();
        ReflText->Indent().AppendF(T("[\"__is_constructor\"] = %s, "), BOOL_STR(Member.Flags & CMF_Constructor)).NewLine();
        ReflText->Indent().AppendF(T("[\"__nilable\"] = %s, "),       BOOL_STR(Member.IsNilable)).NewLine();

        ReflText->Indent().Append(T("[\"__type\"] = ")).Merge(BuildTypeRefl(Member.DeclTypeDesc.Lock(), TextOwner)).NewLine();

        ReflText->IndentDec().Indent().Append(T("}"));

        return true;
    });
    ReflText->NewLine().IndentDec().Indent().Append(T("}")).NewLine();

    ReflText->IndentDec().Indent().Append(T("}, { __index = olua_refl.__class_type_info } )"));
    return ReflText;
}


SPtr<TextParagraph> LPReflHelper::BuildClassMemberType(ClassMemberDesc& Member, TextBuilder& TextOwner)
{
    return BuildTypeRefl(Member.DeclTypeDesc.Lock(), TextOwner);
}

SPtr<TextParagraph> LPReflHelper::BuildTypeRefl(SPtr<TypeDescBase> Type, TextBuilder& TextOwner)
{
    if(Type->ActuallyIs<FuncSigniture>())
    {
        return BuildFunctionType(Type->ActuallyAs<FuncSigniture>(), TextOwner);
    }
    else if(Type->ActuallyIs<ArrayType>())
    {
        return BuildArrayType(Type->ActuallyAs<ArrayType>(), TextOwner);
    }
    else if(Type->ActuallyIs<MapType>())
    {
        return BuildMapType(Type->ActuallyAs<MapType>(), TextOwner);
    }
    else if(Type->IsInt())
    {
        SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("olua_refl.__intrinsic_type_info_int"));
        return Ret;
    }
    else if(Type->IsFloat())
    {
        SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("olua_refl.__intrinsic_type_info_float"));
        return Ret;
    }
    else if(Type->IsString())
    {
        SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("olua_refl.__intrinsic_type_info_string"));
        return Ret;
    }
    else if(Type->IsBool())
    {
        SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("olua_refl.__intrinsic_type_info_bool"));
        return Ret;
    }
    else if(Type->IsAny())
    {
        SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("olua_refl.__intrinsic_type_info_any"));
        return Ret;
    }
    else
    {
            SPtr<TextParagraph> Ret = TextOwner.NewParagraph();
        Ret->Append(T("nil"));
        return Ret;
    }
}

SPtr<TextParagraph> LPReflHelper::BuildFunctionType(SPtr<FuncSigniture> Func, TextBuilder& TextOwner)
{
    SPtr<TextParagraph> FuncText = TextOwner.NewParagraph();
    FuncText->Append(T("setmetatable({")).NewLine().IndentInc();
    if(Func->HasThis)
    {
        FuncText->Indent().Append(T("[\"__self_type\"] = "))
            .Merge(BuildTypeRefl(Func->ThisType.Lock(), TextOwner))
            .Append(T(",")).NewLine();
    }

    // Parameters
    FuncText->Indent().Append(T("[\"__params\"] = {")).NewLine().IndentInc();
    for(int i = 0; i < Func->Params.Count(); i++)
    {
        if(i != 0)
            FuncText->Append(T(",")).NewLine();

        FuncParamDesc& CurrParam = Func->Params[i];
        FuncText->Indent().Append(T("{")).NewLine().IndentInc();

        FuncText->Indent().Append(T("[\"__type\"] = ")).Merge(BuildTypeRefl(CurrParam.Type.Lock(), TextOwner)).Append(T(", ")).NewLine();
        FuncText->Indent().AppendF(T("[\"__nilable\"] = %s, "),   BOOL_STR(CurrParam.IsNilable) ).NewLine();
        FuncText->Indent().AppendF(T("[\"__optional\"] = %s, "),   BOOL_STR(CurrParam.IsOptional) ).NewLine();
        FuncText->Indent().AppendF(T("[\"__const\"] = %s, "),   BOOL_STR(CurrParam.Flags & FPF_Const) ).NewLine();
        FuncText->Indent().AppendF(T("[\"__variable\"] = %s "),   BOOL_STR(CurrParam.IsVariableParam) ).NewLine();

        FuncText->IndentDec().Indent().Append(T("}"));
    }
    if(Func->Params.Count() > 0)
        FuncText->NewLine();

    FuncText->IndentDec().Indent().Append(T("}, ")).NewLine();


    // Returns
    FuncText->Indent().Append(T("[\"__returns\"] = {")).NewLine().IndentInc();
    for(int i = 0; i < Func->Returns.Count(); i++)
    {
        if(i != 0)
            FuncText->Append(T(",")).NewLine();
        FuncReturnDesc& CurrRet = Func->Returns[i];
        FuncText->Indent().Append(T("{")).NewLine().IndentInc();

        FuncText->Indent().AppendF(T("[\"__type\"] = ")).Merge(BuildTypeRefl(CurrRet.Type.Lock(), TextOwner)).Append(T(",")).NewLine();
        FuncText->Indent().AppendF(T("[\"__nilable\"] = %s"), BOOL_STR(CurrRet.IsNilable)).NewLine();

        FuncText->IndentDec().Indent().Append(T("}"));
    }

    if(Func->Returns.Count() > 0)
        FuncText->NewLine();
    FuncText->IndentDec().Indent().Append(T("}")).NewLine();

    FuncText->IndentDec().Indent().Append(T("}, { __index = olua_refl.__function_type_info } )"));
    return FuncText;
}

SPtr<TextParagraph> LPReflHelper::BuildArrayType(SPtr<ArrayType> Array, TextBuilder& TextOwner)
{
    SPtr<TextParagraph> ArrayText = TextOwner.NewParagraph();
    ArrayText->Append(T("setmetatable({")).NewLine().IndentInc();

    ArrayText->Indent().Append(T("[\"__element_type\"] = ")).Merge(BuildTypeRefl(Array->ElemType.Lock(), TextOwner)).Append(T(",")).NewLine();
    ArrayText->Indent().AppendF(T("[\"__element_nilable\"] = %s"), BOOL_STR(Array->IsElemNilable)).NewLine();

    ArrayText->IndentDec().Indent().Append(T("}, { __index = olua_refl.__array_type_info } )"));
    return ArrayText;
}

SPtr<TextParagraph> LPReflHelper::BuildMapType(SPtr<MapType> Map, TextBuilder& TextOwner)
{
    SPtr<TextParagraph> MapText = TextOwner.NewParagraph();
    MapText->Append(T("setmetatable({")).NewLine().IndentInc();

    MapText->Indent().Append(T("[\"__key_type\"] = ")).Merge(BuildTypeRefl(Map->KeyType.Lock(), TextOwner)).Append(T(",")).NewLine();

    MapText->Indent().Append(T("[\"__value_type\"] = ")).Merge(BuildTypeRefl(Map->ValueType.Lock(), TextOwner)).Append(T(",")).NewLine();
    MapText->Indent().AppendF(T("[\"__value_nilable\"] = %s"), BOOL_STR(Map->IsValueNilable)).NewLine();

    MapText->IndentDec().Indent().Append(T("}, { __index = olua_refl.__map_type_info } )"));

    return MapText;
}

}