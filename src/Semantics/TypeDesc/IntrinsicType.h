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
class IntrinsicType : public TypeDescBase
{
    DECLEAR_RTTI()
public:
    EIntrinsicType Type;
    bool IsImplicitAnyType;
    IntrinsicType(EIntrinsicType InType);
    IntrinsicType();
    static SPtr<IntrinsicType> CreateFromRaw(EIntrinsicType RawType, bool InIsImplictAny = false);
    static bool IsIntrinsic(ETypeCat InType);
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString(bool IsNilable);

    virtual bool IsInt();
    virtual bool IsFloat();
    virtual bool IsString();
    virtual bool IsAny();
    virtual bool IsBool();
    virtual bool IsNil();
    virtual bool IsImplicitAny();

    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual bool IsRefType();
    
    virtual OLString DefaultValueText();
private:
    static SPtr<IntrinsicType> TypeInt;
    static SPtr<IntrinsicType> TypeFloat;
    static SPtr<IntrinsicType> TypeString;
    static SPtr<IntrinsicType> TypeBool;
    static SPtr<IntrinsicType> TypeAny;
    static SPtr<IntrinsicType> TypeImplicitAny;
    static SPtr<IntrinsicType> TypeNil;
};

}