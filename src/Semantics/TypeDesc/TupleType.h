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

class ATypeIdentity;

struct TupleSubtype
{
    WPtr<TypeDescBase> Type;
    bool IsNilable;
};
DECLEAR_STRUCT_RTTI(TupleSubtype)


class TupleType : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString(bool IsNilable);

    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    void AddSubtype(SPtr<TypeDescBase> NewSubtype, bool IsNilable);

    OLList<TupleSubtype> Subtypes;

};
}