#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{

class ATypeIdentity;

struct TupleSubtype
{
    WPtr<TypeDescBase> Type;
};

class TupleType : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();
    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    void AddSubtype(SPtr<TypeDescBase> NewSubtype);

    OLList<TupleSubtype> Subtypes;

};
}