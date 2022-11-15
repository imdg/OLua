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
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();

    virtual bool IsInt();
    virtual bool IsFloat();
    virtual bool IsString();
    virtual bool IsAny();
    virtual bool IsBool();
    virtual bool IsNil();
    virtual bool IsImplicitAny();

    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);
    virtual bool IsNilable();
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