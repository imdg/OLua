/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{

class AExpr;
class AEnum;
class AEnumItem;

struct EnumTypeItem
{
    OLString ItemName;
    int      ItemValue;
    WPtr<AExpr> RawValueExpr;
};
DECLEAR_STRUCT_RTTI(EnumTypeItem)

class EnumType : public TypeDescBase
{
    DECLEAR_RTTI();
public:
    virtual ~EnumType();
    void AddItem(SPtr<AEnumItem> Node);
    bool HasItem(OLString ItemName);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString();

    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    int GetItemValue(OLString Item);
    virtual OLString DefaultValueText();
    virtual bool IsRefType();

    OLList<EnumTypeItem>    Items;
    OLString                Name;
};
}